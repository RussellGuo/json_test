#include "InterfaceSetup.h"
#include "USBDevice.h"

static inline bool DeviceSetup(InterfaceSetupStruct* self, USBSetupTransferStruct* transfer, bool isDataPhase);

static void Init(InterfaceSetupStruct* self) {
    USBInterfaceStruct* base = (USBInterfaceStruct*)self;

    base->Init(base, 0, 0, 0, 2, self->InterfaceEPConfig);
}

static bool SetupCallbackIn(InterfaceSetupStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    USBSetupTransferStruct* transfer = &self->Transfer;

    USBInterfaceStruct* base   = (USBInterfaceStruct*)self;
    USBDeviceStruct*    device = base->Device;
    USBHALStruct*       hal    = device->HAL;

    if (buffer == NULL && bytesWritten == 0) {
        // Setup In Process: 1-3 prepare status phase
        // Prepare zero-length out packet (In Process Status)
        hal->EndpointStartRead(hal, self->RxEPConfig.Index, self->RxEPConfig.Buffer, 0);

        // Transfer must be less than or equal to the size requested by the host
        if (transfer->Length >= transfer->Packet.wLength) {
            transfer->IsRequestLength = true;

            transfer->Length = transfer->Packet.wLength;
        }
        else if (transfer->Length == 0 && transfer->Packet.wLength) {
            hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->TxEPConfig.Buffer, 0);
            return true;
        }
    }

    // Setup Out Process: 2-6 status phase completed
    if (transfer->Direction == HOST_TO_DEVICE) {
        // Prepare next setup: Out Process status completed
        hal->EndpointStartSetup(hal);
        if (transfer->Callback) {
            transfer->Callback(device, transfer);
        }
        printf_dbg("**** End Out Process ****\n");
        return true;
    }

    if (transfer->Length == 0) {
        // Setup In Process: 1-5 data completed
        // Send zero-length packet for data stage end
        if (bytesWritten == self->TxEPConfig.MaxPacket && !transfer->IsRequestLength) {
            hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->TxEPConfig.Buffer, 0);
            return true;
        }

        return true;
    }

    // Setup In Process: 1-4 data transferring
    int transferSize = MIN(self->TxEPConfig.MaxBuffer, transfer->Length);

    /* Write to endpoint */
    hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)transfer->Buffer, transferSize);
    /* Update transfer */
    transfer->Buffer += transferSize;
    transfer->Length -= transferSize;
    return true;
}

static inline void DecodeSetupPacket(InterfaceSetupStruct* self, uint8_t* data) {
    USBSetupPacketStruct* packet = &self->Transfer.Packet;
    /* Fill in the elements of a USBSetupPacketStruct structure from raw data */
    packet->bmRequestType.dataTransferDirection = (data[0] & 0x80) >> 7;
    packet->bmRequestType.Type                  = (data[0] & 0x60) >> 5;
    packet->bmRequestType.Recipient             = data[0] & 0x1f;
    packet->bRequest                            = data[1];
    packet->wValue                              = (data[2] | (uint16_t)data[3] << 8);
    packet->wIndex                              = (data[4] | (uint16_t)data[5] << 8);
    packet->wLength                             = (data[6] | (uint16_t)data[7] << 8);
}

static bool SetupCallbackOut(InterfaceSetupStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    USBSetupTransferStruct* transfer = &self->Transfer;

    USBInterfaceStruct* base   = (USBInterfaceStruct*)self;
    USBDeviceStruct*    device = base->Device;
    USBHALStruct*       hal    = device->HAL;

    if (bytesRead & USB_TRANSFER_SETUP) { // for Setup Transfer
        printf_dbg("======== Setup ========\n");
        /* Initialise control transfer state */
        DecodeSetupPacket(self, (uint8_t*)buffer);
        transfer->Buffer    = NULL;
        transfer->Length    = 0;
        transfer->Direction = 0;
        transfer->Callback  = NULL;

        transfer->IsRequestLength = false;

        // Setup In Process: 1-1 setup request
        // Setup Out Process: 2-1 setup request
        if (!DeviceSetup(self, transfer, false)) {
            printf_dbg("The setup request is not supported!\n");
            // Setup In Process: 1-1-1 request not support or failed, stall in data packet
            if (transfer->Packet.bmRequestType.dataTransferDirection == DEVICE_TO_HOST) {
                // Prepare in data stall
                hal->EndpointStall(hal, 0, EPDirectIn, true);
            }
            // Setup Out Process: 2-1-1 request not support or failed, stall out data packet / in status packet (zero-length data)
            else {
                if (transfer->Packet.wLength) {
                    // Prepare out data stall
                    hal->EndpointStall(hal, 0, EPDirectOut, true);
                }
                else {
                    // Prepare in status stall
                    hal->EndpointStall(hal, 0, EPDirectIn, true);
                }
            }
            // Prepare next setup: In/Out Process request not supported
            hal->EndpointStartSetup(hal);
            return true;
        }

        // Transfer must be less than or equal to the size requested by the host
        if (transfer->Length > transfer->Packet.wLength) {
            transfer->Length = transfer->Packet.wLength;
        }

        // only for USB-IF cv test
        /*
        if (!hal->GetAddress(hal)) {
            hal->SetAddress(hal, 0);
        }
        */

        /* Check transfer size and direction */
        if (transfer->Packet.wLength > 0) {
            if (transfer->Direction == DEVICE_TO_HOST) {
                // Setup In Process: 1-2 enter data transfer
                return SetupCallbackIn(self, NULL, 0);
            }
            else {
                // Setup Out Process: 2-2 enter data transfer
                hal->EndpointStartRead(hal, self->RxEPConfig.Index, (uint32_t*)transfer->Buffer, MIN(self->RxEPConfig.MaxBuffer, transfer->Length));
            }
        }
        else {
            // Setup Out Process (Zero-Length): 2-2-1 prepare status phase -> 2-6
            // Prepare zero-length in packet (Out Process Status)
            hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->TxEPConfig.Buffer, 0);
        }
        return true;
    }
    else { // for Setup Out Transfer
        // Setup In Process: 1-6 status phase completed
        if (transfer->Direction == DEVICE_TO_HOST) {
            // Prepare next setup: In Process status completed
            hal->EndpointStartSetup(hal);
            if (transfer->Callback) {
                transfer->Callback(device, transfer);
            }
            printf_dbg("******* End In Process *******\n");
            return true;
        }

        // Setup Out Process: 2-3 data transfer transferring
        bytesRead = MIN(bytesRead, transfer->Length);

        transfer->Buffer += bytesRead;
        transfer->Length -= bytesRead;

        if (transfer->Length) {
            // Meano 20190626 Change to Continue Read
            hal->EndpointStartRead(hal, self->RxEPConfig.Index, (uint32_t*)transfer->Buffer, MIN(self->RxEPConfig.MaxBuffer, transfer->Length));
            return true;
        }

        // Setup Out Process: 2-4 data completed
        if (!DeviceSetup(self, transfer, true)) {
            // Setup Out Process: 2-4-1 out data error or request not support
            printf_dbg("Setup out process request failed!\n");
            hal->EndpointStall(hal, 0, EPDirectIn, true);
            // Prepare next setup: Out process data not supported
            hal->EndpointStartSetup(hal);
            return true;
        }

        // Setup Out Process: 2-5 prepare status phase
        // Prepare zero-length in packet (Out Process Status)
        hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->TxEPConfig.Buffer, 0);
        return true;
    }
}

static inline bool DeviceSetup(InterfaceSetupStruct* self, USBSetupTransferStruct* transfer, bool isDataPhase) {
    USBDeviceStruct* device = self->base.Device;

    if (!device->Setup(device, &self->Transfer))
        return false;

    if (isDataPhase)
        return true;

    if (transfer->Direction != transfer->Packet.bmRequestType.dataTransferDirection)
        return false;

    // Setup Out Process buffer size must large or equal the size that host request
    if (transfer->Direction == HOST_TO_DEVICE && transfer->Length && (transfer->Buffer == NULL || transfer->Length < transfer->Packet.wLength)) {
        printf_dbg("Setup Out Process buffer is less than setup request.\n");
        return false;
    }

    return true;
}

bool InterfaceSetupConstractor(InterfaceSetupStruct* self) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);

    self->Init = Init;

    self->TxEPConfig.Index     = 0;
    self->TxEPConfig.Direct    = EPDirectIn;
    self->TxEPConfig.Type      = EPTypeControl;
    self->TxEPConfig.MaxPacket = 64;
    self->TxEPConfig.MaxBuffer = 64;
    self->TxEPConfig.Buffer    = self->SetupBufferIn;
    self->TxEPConfig.Callback  = (EPCallbackType)SetupCallbackIn;

    self->RxEPConfig.Index     = 0;
    self->RxEPConfig.Direct    = EPDirectOut;
    self->RxEPConfig.Type      = EPTypeControl;
    self->RxEPConfig.MaxPacket = 64;
    self->RxEPConfig.MaxBuffer = 64;
    self->RxEPConfig.Buffer    = self->SetupBufferOut;
    self->RxEPConfig.Callback  = (EPCallbackType)SetupCallbackOut;

    return true;
}
