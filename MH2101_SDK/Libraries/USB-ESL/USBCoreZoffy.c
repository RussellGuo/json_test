#include "USBHAL.h"

#ifdef CONFIG_USB_SUPPORT_CORE_ZOFFY
#include "USBInterface.h"
#include "USBCoreZoffyDefine.h"

// Private Functions Start
static bool ActiveEndpoint(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect);

static void EndpointProcessRead(USBHALStruct* self, uint8_t epIndex, uint32_t bytesToRead);
static void EndpointEndRead(USBHALStruct* self, uint8_t epIndex);

static void EndpointProcessWrite(USBHALStruct* self, uint8_t epIndex);
static void EndpointEndWrite(USBHALStruct* self, uint8_t epIndex);
// Private Functions End

static bool Init(USBHALStruct* self, uint32_t usbBase) {
    self->USBBase = usbBase;

    USBZoffyStruct* usb = self->Zoffy.USB;

    USBTargetEnableModule(self, true);
    USBTargetResetModule(self);

    // Get USB hardware configs from core
    uint32_t cfg = usb->GREGS.GHWCFG2;

    self->DeviceEndpointCount  = ((cfg & USB_GHWCFG2_DEPCOUNT) >> USB_GHWCFG2_DEPCOUNT_Pos) + 1;
    self->HostChannelCount     = (cfg & USB_GHWCFG2_HCHCOUNT) >> USB_GHWCFG2_HCHCOUNT_Pos;
    self->Zoffy.IsDMASupported = ((cfg & USB_GHWCFG2_DMATYPE) >> USB_GHWCFG2_DMATYPE_Pos) == 2;

    cfg &= USB_GHWCFG2_OTGMODE;
    self->Zoffy.IsHostSupported   = cfg < 3 && cfg > 4;
    self->Zoffy.IsDeviceSupported = cfg < 5;
    self->Zoffy.IsSRPSupported    = cfg != 2 && cfg != 6;
    self->Zoffy.IsHNPSupported    = cfg == 0;

    cfg = usb->GREGS.GHWCFG3;

    self->Zoffy.MaxFifoSize = (cfg & USB_GHWCFG3_FIFODEPTH) >> USB_GHWCFG3_FIFODEPTH_Pos;

    cfg = usb->GREGS.GHWCFG4;

    self->Zoffy.IsDedicatedTFifo = (cfg & USB_GHWCFG4_DEDTFIFO) >> USB_GHWCFG4_DEDTFIFO_Pos;

    // Disable Device/Host , SRP/SRP/DMA if is not supported
    self->IsHostEnabled &= self->Zoffy.IsHostSupported;
    self->IsDeviceEnabled &= self->Zoffy.IsDeviceSupported;
    self->IsSRPEnabled &= self->Zoffy.IsSRPSupported;
    self->IsHNPEnabled &= self->Zoffy.IsHNPSupported;
    self->IsDMAEnabled &= self->Zoffy.IsDMASupported;

    // Select Phy by set the default value of GUSBCFG
    cfg = 0;
    switch (self->PhyType) {
        case USBPhyTypeFS:
            cfg |= BIT(6);
            break;
        case USBPhyTypeFSI2C:
            cfg |= BIT(6) | BIT(16);
            break;
        case USBPhyTypeUTMI:
            break;
        case USBPhyTypeULPI:
            cfg |= BIT(4);
            if (self->SpeedConfig != USBSpeedConfigHigh)
                cfg |= BIT(17);
            break;
    }
    usb->GREGS.GUSBCFG = cfg;

    // Enable PHY by target function
    USBTargetEnablePhy(self, true);

    // Reset USB Core after PHY selection
    while (!(usb->GREGS.GRSTCTL & BIT(31))) {}
    usb->GREGS.GRSTCTL = BIT(0);
    while (usb->GREGS.GRSTCTL & BIT(0)) {}

    USBTargetDelayUs(30);

    // Disconnect device first
    usb->DREGS.DCTL = USB_DCTL_DISCONNECT;

    // Turnaround time to maximum value - too small causes packet loss
    usb->GREGS.GUSBCFG = cfg | (0xF << 10);
    if (self->IsSRPEnabled)
        usb->GREGS.GUSBCFG |= BIT(8);

    if (self->IsHNPEnabled)
        usb->GREGS.GUSBCFG |= BIT(9);

    // Enable interrupts
    usb->GREGS.GAHBCFG |= USB_GAHBCFG_GINT; // Global intr unmask           [0]

    // Unmask global interrupts
    usb->GREGS.GINTMSK |= USB_GINTMSK_RXFLVLM | // RFIFO
                          USB_GINTMSK_SOFM |    // SOF
                          USB_GINTMSK_WKUPINT;  // Wakeup

#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
    if (self->IsDMAEnabled) {
        usb->GREGS.GAHBCFG |=
            // BIT(22) |    // All DMA Write Transactions   [22]
            // BIT(21) |    // Remote Memory Support        [21]
            // BIT(8) |     // Periodic TxFEmpLvl           [8] Host?
            USB_GAHBCFG_TXFELVL |         // NPeriodic TxFEmpLvl          [7]
            USB_GAHBCFG_DMAEN |           // DMA Enable                   [5]
            5 << USB_GAHBCFG_HBSTLEN_Pos; // DMA Burst Length             [4:1]
        usb->GREGS.GINTMSK &= ~USB_GINTMSK_RXFLVLM;
    }
#endif

    if (self->IsDeviceEnabled) {
        usb->GREGS.GINTMSK |= USB_GINTMSK_OEPINT |   // OEP
                              USB_GINTMSK_IEPINT |   // IEP
                              USB_GINTMSK_USBRST |   // Reset
                              USB_GINTMSK_USBSUSPM | // Suspend
                              USB_GINTMSK_GONAKEFFM; // GONAKEFFM

        // Device speed
        uint32_t dspd = self->PhyType >= USBPhyTypeUTMI ? (self->SpeedConfig ? USB_DCFG_DSPD_FullSpeed : USB_DCFG_DSPD_HighSpeed) : USB_DCFG_DSPD_FullSpeed48;

        usb->DREGS.DCFG |= dspd |              // Device speed config
                           USB_DCFG_NZLSOHSK | // Non-zero-length status OUT handshake
                           0;
        // Mask In Ep Empty Interrupt
        usb->DREGS.DIEPEMPMSK = 0;
    }

    if (self->IsHostEnabled) {
        usb->GREGS.GINTMSK |= USB_GINTMSK_PORTM |   // Port
                              USB_GINTMSK_CHANNELM; // Channel

        usb->HREGS.HCFG |= self->PhyType ? 0 : 1;
    }

    if (self->IsHostEnabled && self->IsDeviceEnabled)
        usb->GREGS.GINTMSK |= USB_GINTMSK_SRQINTM |  // SRQ
                              USB_GINTMSK_OTGINT |   // OTG
                              USB_GINTMSK_IDCHANGEM; // ID Change

    usb->PCGCCTL = 0x00000000;

    usb->GREGS.GINTSTS = USB_GINTSTS_SOF;

    printf_dbg("USB Inited!\n");
    USBTargetEnableInterrupt(self, true);
    return true;
}

static void Reset(USBHALStruct* self) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    // Set SNAK bits
    for (int i = 0; i < self->DeviceEndpointCount; i++) {
        usb->OEPREGS[i].DOEPCTL = USB_DOEPCTL_SNAK; // SNAK
        usb->IEPREGS[i].DIEPCTL = USB_DIEPCTL_SNAK; // SNAK
    }

    // IN EP Msk
    usb->DREGS.DIEPMSK = USB_DIEPINT_DISABLED | // EP disabled mask
                         USB_DIEPINT_COMPLETED; // Transfer completed mask

    // OUT EP Msk
    usb->DREGS.DOEPMSK = USB_DOEPINT_SETUP |    // Setup mask
                         USB_DOEPINT_DISABLED | // EP disabled mask
                         USB_DOEPINT_COMPLETED; // Transfer completed mask

    // Init Dedicated TFIFO
    if (self->Zoffy.IsDedicatedTFifo) {
        for (int i = 0; i < self->DeviceEndpointCount - 1; i++) {
            usb->GREGS.DIEPTXF[i] = 0;
        }
    }

    // Init RX Fifo Size
    usb->GREGS.GRXFSIZ = USBCORE_ZOFFY_MAX_RX_FIFO_SIZE >> 2;

    self->Zoffy.UsedFifoSize = USBCORE_ZOFFY_MAX_RX_FIFO_SIZE >> 2;
    self->Zoffy.TxFifoIndex  = 0;

    // Clear Address
    usb->DREGS.DCFG &= ~(USB_DCFG_DAD_Msk);

    // Flush RX/TX fifo
    usb->GREGS.GRSTCTL |= BIT(5) | BIT(4);

    // Create the endpoints, and wait for setup packets on out EP0
    ActiveEndpoint(self, 0, EPDirectIn);
    ActiveEndpoint(self, 0, EPDirectOut);
}

static void StartDevice(USBHALStruct* self, USBDeviceStruct* device) {
    self->Device        = device;
    USBZoffyStruct* usb = self->Zoffy.USB;
    usb->DREGS.DCTL     = 0;
    printf_dbg("USB Device Started!\n");
}

static void StopDevice(USBHALStruct* self) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    usb->DREGS.DCTL = 2;
    printf_dbg("USB Device Stoped!\n");
}

static bool ActiveEndpoint(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    EPStruct* ep = &self->EP[epIndex][epDirect];

    EPConfigStruct* epConfig = ep->Config;

    uint32_t epReg = USB_DIEPCTL_SD0PID |                         // Set the EP specific control settings (SD0PID)
                     USB_DIEPCTL_SNAK |                           // SNAK
                     (ep->Config->Type << USB_DIEPCTL_TYPE_Pos) | // Endpoint type
                     USB_DIEPCTL_ACTIVE |                         // Active endpoint
                     (epIndex ? (ep->Config->MaxPacket) : 0);     // Single Packet Max Transfer Size

    ep->Status = EPCompleted;
    if (epDirect == EPDirectIn) { // In Endpoint
        if (self->Zoffy.IsDedicatedTFifo) {
            if (epIndex == 0) { // Set up the Tx FIFO
                usb->GREGS.GNPTXFSIZ = ((epConfig->MaxPacket >> 2) << 16) | (self->Zoffy.UsedFifoSize << 0);
            }
            else {
                usb->GREGS.DIEPTXF[self->Zoffy.TxFifoIndex - 1] = ((epConfig->MaxPacket >> 2) << 16) | (self->Zoffy.UsedFifoSize << 0);
            }
            self->Zoffy.UsedFifoSize += epConfig->MaxPacket >> 2;
            epReg |= (self->Zoffy.TxFifoIndex << 22); // TxFIFO index
            self->Zoffy.TxFifoIndex += 1;
        }
        else {
            uint32_t inMaxPacket = MAX((self->Zoffy.UsedFifoSize << 2) - USBCORE_ZOFFY_MAX_RX_FIFO_SIZE, epConfig->MaxPacket);
            usb->GREGS.GNPTXFSIZ = ((inMaxPacket >> 2) << 16) | (USBCORE_ZOFFY_MAX_RX_FIFO_SIZE >> 2);

            self->Zoffy.UsedFifoSize = (USBCORE_ZOFFY_MAX_RX_FIFO_SIZE + inMaxPacket) >> 2;
        }

        // Unmask the interrupt
        usb->DREGS.DAINTMSK |= BIT(epIndex);

        // Clear endpoint interrupt reg
        usb->IEPREGS[epIndex].DIEPINT = USB_DIEPINT_COMPLETED | USB_DIEPINT_DISABLED;
        // Set endpoint control reg
        usb->IEPREGS[epIndex].DIEPCTL = epReg;

        if (self->Zoffy.UsedFifoSize > self->Zoffy.MaxFifoSize)
            return false;
    }
    else { // Out endpoint
        // Enable Out EP split package when `epConfig->Timeout` valid
        if (epConfig->Timeout && epConfig->MaxBuffer > epConfig->MaxPacket) {
            self->Zoffy.OEPSplitMask |= BIT(epIndex);
        }

        // Unmask the interrupt
        usb->DREGS.DAINTMSK |= BIT((epIndex + 16));

        // Clear endpoint interrupt reg
        usb->OEPREGS[epIndex].DOEPINT = USB_DOEPINT_COMPLETED | USB_DOEPINT_DISABLED | USB_DOEPINT_SETUP | USB_DOEPINT_STATUS | USB_DOEPINT_SETUP_PACKET;
        // Set endpoint control reg
        usb->OEPREGS[epIndex].DOEPCTL = epReg;

        // Ready first read for EP
        if (epIndex == 0)
            self->EndpointStartSetup(self);
        else
            self->EndpointStartRead(self, epIndex, epConfig->Buffer, epConfig->MaxBuffer);
    }
    return true;
}

static uint8_t GetAddress(USBHALStruct* self) {
    return self->Zoffy.USB->DREGS.DCFG & USB_DCFG_DAD >> USB_DCFG_DAD_Pos;
}

static void SetAddress(USBHALStruct* self, uint8_t address) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    uint32_t dcfgReg = usb->DREGS.DCFG & (~USB_DCFG_DAD);

    usb->DREGS.DCFG = (dcfgReg | (address << 4));
}

static bool ConfigureDevice(USBHALStruct* self) {
    for (int epIndex = 1; epIndex < self->DeviceEndpointCount; epIndex++) {
        EPStruct* ep = &self->EP[epIndex][EPDirectIn];
        if (ep->Config && (ActiveEndpoint(self, epIndex, EPDirectIn) == false)) {
            return false;
        }

        ep = &self->EP[epIndex][EPDirectOut];
        if (ep->Config && (ActiveEndpoint(self, epIndex, EPDirectOut) == false)) {
            return false;
        }
    }
    return true;
}

static void EndpointStall(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect, bool isEnable) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    EPStruct* ep = &self->EP[epIndex][epDirect];

    if (isEnable) {
        if (epDirect == EPDirectOut) {
            usb->OEPREGS[epIndex].DOEPCTL |= USB_DOEPCTL_STALL; // In Stall
        }
        else {
            usb->IEPREGS[epIndex].DIEPCTL |= USB_DIEPCTL_STALL; // Out Stall
        }

        ep->Status = EPStalled;
        printf_dbg("Stall ep %d,%d\n", epIndex, epDirect);
    }
    else {
        if (epDirect == EPDirectOut) {
            usb->OEPREGS[epIndex].DOEPCTL |= USB_DOEPCTL_SD0PID;
            usb->OEPREGS[epIndex].DOEPCTL &= ~USB_DOEPCTL_STALL; // In Stall
        }
        else {
            usb->OEPREGS[epIndex].DOEPCTL |= USB_DIEPCTL_SD0PID;
            usb->IEPREGS[epIndex].DIEPCTL &= ~USB_DIEPCTL_STALL; // Out Stall
        }

        ep->Status = EPCompleted;
        printf_dbg("Unstall ep %d,%d\n", epIndex, epDirect);
    }
}

static void SetTestMode(USBHALStruct* self, uint8_t testMode) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    uint32_t dctlReg = usb->DREGS.DCTL & (~USB_DCTL_TCTL);
    switch (testMode) {
        case TEST_Packet:
        case TEST_J:
        case TEST_K:
        case TEST_SE0_NAK:
        case TEST_Force_Enable:
            dctlReg |= ((testMode) << USB_DCTL_TCTL_Pos);
            break;
    }
    usb->DREGS.DCTL = dctlReg;
}

static bool EndpointStartProbe(USBHALStruct* self, uint8_t epIndex) {
    EPStruct* ep = &self->EP[epIndex][EPDirectIn];

    if (ep->Status != EPCompleted)
        return false;

    self->IEPProbingMask |= BIT(epIndex);
    ep->PendingTick = 0;
    ep->Status      = EPProbing;

    return true;
}

static EPStatus GetEPStatus(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect) {
    return self->EP[epIndex][epDirect].Status;
}

static USBSpeedEnum GetCurrentSpeed(USBHALStruct* self) {
    USBSpeedEnum speed = (USBSpeedEnum)(self->Zoffy.USB->DREGS.DSTS & 0x00000006 >> 1);
    return speed > USBSpeedLow ? USBSpeedFull : speed;
}

/*
// Only availabled when ep in EPPending status
static inline uint32_t GetCurrentReadSize(USBHALStruct* self, uint8_t epIndex) {
    EPStruct* ep = &self->EP[epIndex][EPDirectOut];
    return ep->Status == EPPending ? ep->TransferSize - self->Zoffy.USB->OEPREGS[epIndex].DOEPTSIZ & 0x7FFFF : 0;
}
*/

static void EndpointStartSetup(USBHALStruct* self) {
    EPStruct* ep = &self->EP[0][EPDirectOut];
    ep->IsSetup  = true;
    self->EndpointStartRead(self, 0, ep->Config->Buffer, ep->Config->MaxBuffer);
}

static bool EndpointStartRead(USBHALStruct* self, uint8_t epIndex, uint32_t* epBuffer, uint32_t bytesToRead) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    EPStruct* ep = &self->EP[epIndex][EPDirectOut];

    uint32_t epReg = usb->OEPREGS[epIndex].DOEPCTL;

    epReg |= USB_DOEPCTL_EPENA | USB_DOEPCTL_CNAK; // Enable endpoint | Clear NAK

    uint32_t epMaxPacketSize = ep->Config->MaxPacket;
    bytesToRead |= MAX((bytesToRead + epMaxPacketSize - 1) / epMaxPacketSize, 1) << 19;
    if (ep->Config->Type == EPTypeControl) {
        bytesToRead |= (3 << 29); // 3 setup packet
    }

    ep->Buffer = epBuffer; // Record Read Buffer Addr
#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
    if (self->IsDMAEnabled)
        usb->OEPREGS[epIndex].DOEPDMA = (uint32_t)epBuffer;
#endif

    ep->Buffer         = epBuffer;
    ep->TransferSize   = bytesToRead & 0x7FFFF; // Clear TransferSize
    ep->TransferOffset = 0;
    ep->PendingTick    = 0;
    ep->Status         = EPPending;

    usb->OEPREGS[epIndex].DOEPTSIZ = bytesToRead;
    usb->OEPREGS[epIndex].DOEPCTL |= epReg;

    self->OEPPendingMask |= BIT(epIndex);

    printf_dbg("SRead EP%d,%d.\n", epIndex, ep->TransferSize);
    return true;
}

// for Slave Mode
static inline void EndpointProcessRead(USBHALStruct* self, uint8_t epIndex, uint32_t bytesToRead) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    EPStruct* ep = &self->EP[epIndex][EPDirectOut];

    uint32_t* bufferPoint = &ep->Buffer[ep->TransferOffset >> 2];
    for (uint32_t i = 0; i < ToUintSize(bytesToRead); i++) {
        bufferPoint[i] = usb->FIFO[0][0];
    }
    ep->TransferOffset += bytesToRead;
    return;
}

static void EndpointEndRead(USBHALStruct* self, uint8_t epIndex) {
    EPStruct* ep = &self->EP[epIndex][EPDirectOut];

    ep->TransferSize -= self->Zoffy.USB->OEPREGS[epIndex].DOEPTSIZ & 0x7FFFF;

    // OUT Transfer Completed
    ep->Status = EPCompleted;
    // Setup Done
    ep->IsSetup = false;

    self->OEPPendingMask &= ~BIT(epIndex);

    if (ep->Config->Callback && !(*ep->Config->Callback)(ep->Config->Interface, ep->Buffer, ep->TransferSize)) {
        ep->Status = EPRetrying;
        self->OEPRetryMask |= BIT(epIndex);
    }
}

/*
// Only availabled when ep in EPPending status
static inline uint32_t GetCurrentWriteSize(USBHALStruct* self, uint8_t epIndex) {
    EPStruct* ep = &self->EP[epIndex][EPDirectIn];
    return ep->Status == EPPending ? ep->TransferSize - (self->Zoffy.USB->IEPREGS[epIndex].DIEPTSIZ & 0x7FFFF) : 0;
}
*/

static bool EndpointStartWrite(USBHALStruct* self, uint8_t epIndex, uint32_t* epBuffer, uint32_t bytesToWrite) {
    printf_dbg("SWrite EP%d,%d.\n", epIndex, bytesToWrite);
    USBZoffyStruct* usb = self->Zoffy.USB;
    EPStruct*       ep  = &self->EP[epIndex][EPDirectIn];

    // Change epBuffer to backup buffer when epBuffer is unaligned (4-byte) or not in RAM space when DMA Enabled
    if (bytesToWrite && //
        ((uint32_t)epBuffer % 4
#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
         || (uint32_t)epBuffer < 0x20000000
#endif
         )) {
        printf_dbg("Change to backup buffer.\n");
        util_assert(ep->Config->Buffer);
        bytesToWrite = MIN(bytesToWrite, ep->Config->MaxBuffer);
        memcpy(ep->Config->Buffer, epBuffer, bytesToWrite);
        epBuffer = ep->Config->Buffer;
    }
#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
    if (self->IsDMAEnabled)
        usb->IEPREGS[epIndex].DIEPDMA = (uint32_t)epBuffer;
#endif
    uint32_t epMaxPacketSize = ep->Config->MaxPacket;

    usb->IEPREGS[epIndex].DIEPTSIZ = MAX((bytesToWrite + epMaxPacketSize - 1) / epMaxPacketSize, 1) << 19 | // multi packet
                                     (bytesToWrite << 0);                                                   // Size of packet

    uint32_t epReg = usb->IEPREGS[epIndex].DIEPCTL;
    epReg |= USB_DIEPCTL_EPENA | USB_DIEPCTL_CNAK; // Enable endpoint | Clear NAK
    usb->IEPREGS[epIndex].DIEPCTL = epReg;

    ep->Buffer         = epBuffer;
    ep->TransferSize   = bytesToWrite & 0x7FFFF;
    ep->TransferOffset = 0;
    ep->Status         = EPPending;

    self->IEPPendingMask |= BIT(epIndex);

#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
    if (self->IsDMAEnabled)
        return true;
#endif

    if (self->Zoffy.IsDedicatedTFifo)
        usb->DREGS.DIEPEMPMSK |= BIT(epIndex);
    else
        usb->DREGS.DIEPMSK |= USB_DIEPINT_TOKEN; // In token received when txfifo emptyz

    return true;
}

// for Slave Mode
static inline void EndpointProcessWrite(USBHALStruct* self, uint8_t epIndex) {
    USBZoffyStruct* usb = self->Zoffy.USB;
    EPStruct*       ep  = &self->EP[epIndex][EPDirectIn];

    if (ep->Status != EPPending)
        return;

    uint32_t gnptxstsReg = usb->GREGS.GNPTXSTS;
    if (((gnptxstsReg & BITS(23, 16)) >> 16) < 8) {
        return;
    }

    uint32_t  bytesToWrite = MIN((ep->TransferSize - ep->TransferOffset), ep->Config->MaxPacket);
    uint32_t* bufferPoint  = ep->Buffer + (ep->TransferOffset >> 2);
    for (uint32_t i = 0; i < ToUintSize(bytesToWrite); i++) {
        usb->FIFO[epIndex][0] = bufferPoint[i];
    }

    ep->TransferOffset += bytesToWrite;

    return;
}

static void EndpointEndWrite(USBHALStruct* self, uint8_t epIndex) {
    USBZoffyStruct* usb = self->Zoffy.USB;
    EPStruct*       ep  = &self->EP[epIndex][EPDirectIn];
    // IN Transfer Completed
    ep->Status = EPCompleted;
    self->IEPPendingMask &= ~BIT(epIndex);

    if (!self->IsDMAEnabled) {
        // Stop firing Tx empty interrupts
        // Will get turned on again if another write is called
        if (self->Zoffy.IsDedicatedTFifo)
            usb->DREGS.DIEPEMPMSK &= ~BIT(epIndex);
        else if (!self->IEPPendingMask)
            usb->DREGS.DIEPMSK &= ~USB_DIEPINT_TOKEN; // In token received when txfifo empty
    }

    if (ep->Config->Callback == NULL || !(ep->Config->Callback)(ep->Config->Interface, ep->Buffer, ep->TransferSize)) {
        self->EndpointStall(self, epIndex, EPDirectIn, true);
    }
}

static void TickTask(USBHALStruct* self) {
    uint32_t epIndex = 0;

    uint16_t oEPPendingMask = (self->OEPPendingMask & self->Zoffy.OEPSplitMask) | self->OEPRetryMask;

    while (oEPPendingMask >>= 1) {
        epIndex++;
        if (!(oEPPendingMask & 1))
            continue;

        EPStruct* ep = &self->EP[epIndex][EPDirectOut];

        // Retry out endpoint callback
        if (ep->Status == EPRetrying) {
            if ((*ep->Config->Callback)(ep->Config->Interface, ep->Buffer, ep->TransferSize)) {
                self->OEPRetryMask &= ~BIT(epIndex);
            }
            continue;
        }

        // Check if necessary to split package for out endpoints
        if (self->Zoffy.OEPToDisable)
            continue;

        USBZoffyStruct* usb = self->Zoffy.USB;

        uint32_t readSize = ep->TransferSize - usb->OEPREGS[epIndex].DOEPTSIZ & 0x7FFFF;

        if (readSize % ep->Config->MaxBuffer / ep->Config->MaxPacket == 0)
            continue;

        ep->PendingTick++;
        if (ep->PendingTick < ep->Config->Timeout)
            continue;

        // The endpoint will disabled when Global OUT NAK take effect
        self->Zoffy.OEPToDisable = epIndex;
        usb->DREGS.DCTL |= USB_DCTL_SGONAK; // Set Global OUT NAK
        break;
    }

    epIndex = 0;

    uint16_t iEPPendingMask = self->IEPProbingMask;
    while (iEPPendingMask >>= 1) {
        epIndex++;
        if (!(iEPPendingMask & 1))
            continue;

        // IN EP Probe Task
        EPStruct* ep = &self->EP[epIndex][EPDirectIn];
        ep->PendingTick++;
        if (ep->Status == EPProbing) {
            USBZoffyStruct* usb = self->Zoffy.USB;
            if (ep->PendingTick < ep->Config->Timeout) {
                usb->IEPREGS[epIndex].DIEPINT |= USB_DIEPINT_NAK;
                continue;
            }
            else if (usb->IEPREGS[epIndex].DIEPINT & USB_DIEPINT_NAK) {
                if (ep->Config->Callback)
                    (ep->Config->Callback)(ep->Config->Interface, NULL, 0);
            }
            else if (ep->PendingTick < ep->Config->Timeout * 2)
                continue;
            ep->Status = EPCompleted;
        }
        self->IEPProbingMask &= ~BIT(epIndex);
    }
}

static inline void DeviceInterrupt(USBHALStruct* self, USBZoffyStruct* usb, uint32_t gintsts) {
    if (gintsts & USB_GINTSTS_USBSUSP) { // USB Suspend [R/W1C]
        printf_dbg("USB Int Suspend.\n");
        if (self->BusCallback)
            self->BusCallback(self, USBBusSuspendEvent);
        usb->GREGS.GINTSTS = USB_GINTSTS_USBSUSP; // Clear USB Suspend
    }

    if (gintsts & USB_GINTSTS_USBRST) { // USB Reset [R/W1C]
        Reset(self);
        printf_dbg("USB Int Reset.\n");
        if (self->BusCallback)
            self->BusCallback(self, USBBusResetEvent);
        usb->GREGS.GINTSTS = USB_GINTSTS_USBRST; // Clear USB Reset
    }

    if (gintsts & USB_GINTSTS_OEPINT) { // [O] endpoint interrupt [R]
        uint32_t epIntMask = usb->DREGS.DAINT >> USB_DAINT_OEPINT_Pos;
        uint32_t epIndex   = 0;
        while (epIntMask) {
            if (epIntMask & 1) {
                uint32_t outEpInt = usb->OEPREGS[epIndex].DOEPINT;

                EPStruct* ep = &self->EP[epIndex][EPDirectOut];

                if (outEpInt & USB_DOEPINT_SETUP) {
                    usb->OEPREGS[epIndex].DOEPINT = USB_DOEPINT_COMPLETED | USB_DOEPINT_SETUP | USB_DOEPINT_SETUP_PACKET;
                    ep->TransferSize |= USB_TRANSFER_SETUP;
                    EndpointEndRead(self, epIndex);
                }
                else if (outEpInt & USB_DOEPINT_COMPLETED) {
                    usb->OEPREGS[epIndex].DOEPINT = USB_DOEPINT_COMPLETED;
                    EndpointEndRead(self, epIndex);
                }

                if (outEpInt & USB_DOEPINT_DISABLED) {
                    usb->OEPREGS[epIndex].DOEPINT = USB_DOEPINT_DISABLED;
                    if (ep->Status == EPPending)
                        EndpointEndRead(self, epIndex);
                    self->Zoffy.OEPToDisable = 0;
                    usb->DREGS.DCTL |= USB_DCTL_CGONAK;
                }

                printf_dbg("OINT%d %08X\n", epIndex, outEpInt);
            }
            epIntMask >>= 1;
            epIndex++;
        }
    }

    if (!self->IsDMAEnabled && (gintsts & USB_GINTSTS_RXFLVL)) { // [O] RX FIFO not empty [R]
        volatile uint32_t epRxStatus = usb->GREGS.GRXSTSR;

        uint32_t type    = (epRxStatus >> 17) & 0xF;
        uint32_t epIndex = epRxStatus & 0xF;
        uint32_t rxSize  = (epRxStatus >> 4) & 0x7FF;

        EPStruct* ep = &self->EP[epIndex][EPDirectOut];
        if (type == 0x6) { // Setup packet received
            if (ep->IsSetup) {
                epRxStatus = usb->GREGS.GRXSTSP;
                EndpointProcessRead(self, epIndex, rxSize);
                printf_dbg("Setup Packet: ");
            }
            else if (ep->PendingTick++ > 16) {
                // TODO: IN/OUT Process should be cleared
                self->EndpointStartSetup(self);
            }
        }
        else if (type == 0x2) { // Out packet received
            epRxStatus = usb->GREGS.GRXSTSP;
            EndpointProcessRead(self, epIndex, rxSize);
        }
        else {
            epRxStatus = usb->GREGS.GRXSTSP;
        }
        printf_dbg("RINT %d,%d\n", type, rxSize);
    }

    if (gintsts & USB_GINTSTS_IEPINT) { // [I] endpoint interrupt [R]
        uint32_t epIntMask = usb->DREGS.DAINT & USB_DAINT_IEPINT;
        uint32_t epIndex   = 0;
        while (epIntMask) {
            if (epIntMask & 1) {
                uint32_t inEpInt = usb->IEPREGS[epIndex].DIEPINT;
                printf_dbg("IINT%d %08X\n", epIndex, inEpInt);

                if (inEpInt & USB_DIEPINT_COMPLETED) { // Tx transfer is complete
                    usb->IEPREGS[epIndex].DIEPINT = USB_DIEPINT_COMPLETED;
                    EndpointEndWrite(self, epIndex);
                }

#if CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
                if (self->IsDMAEnabled) {
                    epIntMask >>= 1;
                    epIndex++;
                    continue;
                }
#endif

                if (self->Zoffy.IsDedicatedTFifo && (inEpInt & USB_DIEPINT_FIFOEMPTY)) { // TXFIFO empty
                    EndpointProcessWrite(self, epIndex);
                }
                else if (inEpInt & USB_DIEPINT_TOKEN) { // In token received when TXFIFO empty
                    EndpointProcessWrite(self, epIndex);
                    usb->IEPREGS[epIndex].DIEPINT = USB_DIEPINT_TOKEN;
                }
            }
            epIntMask >>= 1;
            epIndex++;
        }
    }

    if (gintsts & USB_GINTSTS_SOF) { // Start of frame
        TickTask(self);
        if (self->SofCallback)
            self->SofCallback(self);
        usb->GREGS.GINTSTS = USB_GINTSTS_SOF;
    }

    if (gintsts & USB_GINTSTS_GONAKEFF) { // Global out nak [R]
        printf_dbg("USB Global Out Nak.\n");
        if (self->Zoffy.OEPToDisable) {
            usb->OEPREGS[self->Zoffy.OEPToDisable].DOEPCTL |= USB_DOEPCTL_SNAK | USB_DOEPCTL_EPDIS;
        }
        else {
            usb->DREGS.DCTL |= USB_DCTL_CGONAK;
        }
    }

    if (gintsts & USB_GINTSTS_ENUMDNE) { // Enum Down
        printf_dbg("USB int EnumDown. %08X\n", usb->DREGS.DSTS);
        usb->GREGS.GINTSTS = USB_GINTSTS_ENUMDNE;

        self->Speed = (USBSpeedEnum)(self->Zoffy.USB->DREGS.DSTS & 0x00000006 >> 1);
        self->Speed = self->Speed > USBSpeedLow ? USBSpeedFull : self->Speed;
    }

    if (gintsts & USB_GINTSTS_SRQINT) {
        printf_dbg("USB Device SRQ.\n");
        usb->GREGS.GINTSTS = USB_GINTSTS_SRQINT;
    }

    if (gintsts & USB_GINTSTS_WKUPINT) { // USB Resume [R/W1C]
        printf_dbg("USB Int Resume.\n");
        if (self->BusCallback)
            self->BusCallback(self, USBBusResumeEvent);
        usb->GREGS.GINTSTS = USB_GINTSTS_WKUPINT; // Clear USB Resume
    }
}

static inline void HostInterrupt(USBHALStruct* self, USBZoffyStruct* usb, uint32_t gintsts) {
    if (gintsts & USB_GINTSTS_SRQINT) {
        printf_dbg("Host SRQ\n");
        if (!(usb->HREGS.HPRT & BIT(12)))
            usb->HREGS.HPRT |= BIT(12);
        usb->GREGS.GINTSTS = USB_GINTSTS_SRQINT;
    }

    if (gintsts & USB_GINTSTS_PORT) {
        uint32_t hprt = usb->HREGS.HPRT;
        printf_dbg("HPort Int %08X\n", hprt);
        if (hprt & BIT(1)) {
            printf_dbg("HPort Connected!\n");
        }
        if (hprt & BIT(3)) {
            printf_dbg("HPort Changed %lu\n", hprt & BIT(2));
        }
        if (hprt & BIT(5)) {
            printf_dbg("HPort OverCurr %lu\n", hprt & BIT(4));
        }

        usb->HREGS.HPRT = hprt;
    }

    if (gintsts & USB_GINTSTS_SOF) {
        usb->GREGS.GINTSTS |= USB_GINTSTS_SOF;
    }
}

static void Interrupt(USBHALStruct* self) {
    USBZoffyStruct* usb = self->Zoffy.USB;

    uint32_t gintsts = usb->GREGS.GINTSTS;

    if (gintsts & USB_GINTSTS_MODE) {
        HostInterrupt(self, usb, gintsts);
    }
    else {
        DeviceInterrupt(self, usb, gintsts);
    }
}

bool USBCoreZoffyConstractor(USBHALStruct* self) {
    self->Init = Init;

    self->StartDevice     = StartDevice;
    self->StopDevice      = StopDevice;
    self->GetAddress      = GetAddress;
    self->SetAddress      = SetAddress;
    self->ConfigureDevice = ConfigureDevice;

    self->SetTestMode     = SetTestMode;
    self->GetEPStatus     = GetEPStatus;
    self->GetCurrentSpeed = GetCurrentSpeed;

    self->EndpointStartSetup = EndpointStartSetup;
    self->EndpointStartProbe = EndpointStartProbe;
    self->EndpointStall      = EndpointStall;
    self->EndpointStartRead  = EndpointStartRead;
    self->EndpointStartWrite = EndpointStartWrite;

    self->Interrupt = Interrupt;

    self->IsDeviceEnabled = true;
    return true;
}

#endif // CONFIG_USB_SUPPORT_CORE_ZOFFY
