#include "InterfaceMSD.h"
#include "USBDevice.h"

static const uint8_t DefaultInquiry[] __ALIGNED(4) = {
    0x00, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,       //
    'M',  'e',  'g',  'a',  'H',  'u',  'n',  't',        //
    'V',  'F',  'S',  'D',  'i',  's',  'k',              //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
    '1',  '.',  '0',  0x00,
};

// private functions
static inline void CBWDecode(InterfaceMSDStruct* self, uint32_t* buffer, uint16_t size);
static inline void CBWOutProcess(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesRead);
static inline void CBWInProcess(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesWritten);

static void Init(InterfaceMSDStruct* self) {
    USBInterfaceStruct* base = &self->base;

    self->Stage = READ_CBW;

    self->CSW = (CSWStruct) {0};
    self->CBW = (CBWStruct) {0};

    self->MemorySize = self->BlockCount * self->BlockSize;

    self->RxEPConfig.MaxBuffer = self->WriteBufferSize;
    self->RxEPConfig.Buffer    = self->WriteBuffer;

    self->TxEPConfig.MaxBuffer = self->ReadBufferSize;
    self->TxEPConfig.Buffer    = self->ReadBuffer;

    base->Init(base, 0x08, 0x06, 0x50, 2, self->InterfaceEPConfig);
}

static bool ClassRequestCallback(InterfaceMSDStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    if (transfer->Packet.bmRequestType.Type == CLASS_TYPE) {
        switch (transfer->Packet.bRequest) {
            case MSC_REQUEST_RESET:
                // Reset stage to read CBW
                self->Stage = READ_CBW;
                success     = true;
                break;
            case MSC_REQUEST_GET_MAX_LUN:
                transfer->Length    = 1;
                transfer->Buffer    = self->MaxLun;
                transfer->Direction = DEVICE_TO_HOST;
                success             = true;
                break;
            default:
                break;
        }
    }

    return success;
}

static bool IEPCallback(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    switch (self->Stage) {
        // the device has to send data to the host
        case PROCESS_CBW:
            printf_dbg("pr\n");

            // For custom SCSI command
            if (self->InProcessCallback && self->InProcessCallback(self, (uint8_t*)buffer, bytesWritten))
                break;
            CBWInProcess(self, buffer, bytesWritten);
            break;

        // the device has to send a CSW
        case SEND_CSW:
            printf_dbg("se\n");
            self->SendCSW(self);
            break;

        // the host has received the CSW -> we wait a CBW
        case WAIT_CSW:
            printf_dbg("w\n");
            self->Stage = READ_CBW;
            break;

        // an error has occured
        default:
            printf_dbg("s\n");
            // stallEndpoint(EPBULK_IN);
            self->SendCSW(self);
            break;
    }
    return true;
}

static bool OEPCallback(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    uint32_t  nextLength = self->WriteBufferSize;
    uint32_t* nextBuffer = self->WriteBuffer;
    switch (self->Stage) {
        // the device has to decode the CBW received
        case WAIT_CSW:
        case READ_CBW:
            CBWDecode(self, buffer, bytesRead);
            break;

        // the device has to receive data from the host
        case PROCESS_CBW:
            // For custom SCSI command
            if (self->OutProcessCallback && self->OutProcessCallback(self, (uint8_t*)buffer, bytesRead))
                break;

            CBWOutProcess(self, buffer, bytesRead);
            break;

        // an error has occured: stall endpoint and send CSW
        default:
            // stallEndpoint(2);
            self->CSW.Status = CSW_ERROR;
            self->SendCSW(self);
            break;
    }

    if (self->IsPendingRead)
        return false;

    if ((self->CBW.Flags & 0x80) == 0 && self->CSW.DataResidue) {
        nextLength = MIN(self->CSW.DataResidue, nextLength);
    }
    else {
        nextLength = sizeof(CBWStruct);
        nextBuffer = self->CBWBuffer;
    }

    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    hal->EndpointStartRead(hal, self->RxEPConfig.Index, nextBuffer, nextLength);
    return true;
}

static void Fail(InterfaceMSDStruct* self) {
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    self->Stage       = SEND_CSW;

    if (!hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->ReadBuffer, 0)) {
        return;
    }
    self->CSW.Status = CSW_FAILED;
}

static void SendCSW(InterfaceMSDStruct* self) {
    USBHALStruct* hal   = self->base.Device->HAL;
    self->CSW.Signature = CSW_Signature;
    self->Stage         = WAIT_CSW;
    hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)&self->CSW, sizeof(self->CSW));
}

static bool Write(InterfaceMSDStruct* self, uint32_t* writeBuffer, uint16_t writeLength) {
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    if (writeLength >= self->CBW.DataLength) {
        writeLength = self->CBW.DataLength;
    }
    self->Stage = SEND_CSW;

    if (!hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)writeBuffer, writeLength)) {
        return false;
    }

    self->CSW.DataResidue -= writeLength;
    self->CSW.Status = CSW_PASSED;
    return true;
}

static void MemoryWrite(InterfaceMSDStruct* self, uint32_t* writeBuffer, uint32_t writeLength) {
    // `writeLength % self->BlockSize` may be needed to check
    if ((self->Addr + writeLength) > self->MemorySize) {
        writeLength = 0;
        self->Stage = STAGE_ERROR;
    }

    if (!(self->DiskStatusCallback(self) && WRITE_PROTECT))
        self->DiskWriteCallback(self, (uint8_t*)writeBuffer, self->Addr / self->BlockSize, writeLength / self->BlockSize);

    self->Addr += writeLength;
    self->Length -= writeLength;
    self->CSW.DataResidue -= writeLength;

    if ((!self->Length) || (self->Stage != PROCESS_CBW)) {
        self->CSW.Status = (self->Stage == STAGE_ERROR) ? CSW_ERROR : CSW_PASSED;
        self->SendCSW(self);
    }
}

static void MemoryVerify(InterfaceMSDStruct* self, uint32_t* verifyBuffer, uint32_t verifyLength) {
    // `verifyLength % self->BlockSize` may be needed to check
    if ((self->Addr + verifyLength) > self->MemorySize) {
        verifyLength = 0;
        self->Stage  = STAGE_ERROR;
    }

    // TODO: ReadBufferSize may little than verifyLength
    if (!(self->Addr % self->BlockSize) && self->DiskReadCallback)
        self->DiskReadCallback(self, (uint8_t*)self->ReadBuffer, self->Addr / self->BlockSize, verifyLength / self->BlockSize);

    // Compare verifyBuffer with ReadBuffer
    for (uint32_t i = 0; i < verifyLength / 4; i++) {
        if (self->ReadBuffer[i] != verifyBuffer[i]) {
            self->MemOK = false;
            break;
        }
    }

    self->Addr += verifyLength;
    self->Length -= verifyLength;
    self->CSW.DataResidue -= verifyLength;

    if (!self->Length || (self->Stage != PROCESS_CBW)) {
        self->CSW.Status = (self->Stage == STAGE_ERROR) ? CSW_ERROR : (self->MemOK ? CSW_PASSED : CSW_FAILED);
        self->SendCSW(self);
    }
}

static bool InquiryRequest(InterfaceMSDStruct* self) {
    memcpy(self->ReadBuffer, self->InquiryData, self->InquirySize);
    if (!self->Write(self, self->ReadBuffer, self->InquirySize)) {
        return false;
    }
    return true;
}

static bool ReadFormatCapacity(InterfaceMSDStruct* self) {
    uint8_t capacity[] = {
        0x00,
        0x00,
        0x00,
        0x08,
        (uint8_t)((self->BlockCount >> 24) & 0xff),
        (uint8_t)((self->BlockCount >> 16) & 0xff),
        (uint8_t)((self->BlockCount >> 8) & 0xff),
        (uint8_t)((self->BlockCount >> 0) & 0xff),
        0x02,
        (uint8_t)((self->BlockSize >> 16) & 0xff),
        (uint8_t)((self->BlockSize >> 8) & 0xff),
        (uint8_t)((self->BlockSize >> 0) & 0xff),
    };
    memcpy(self->ReadBuffer, capacity, sizeof(capacity));
    if (!self->Write(self, self->ReadBuffer, sizeof(capacity))) {
        return false;
    }
    return true;
}

static bool ReadCapacity(InterfaceMSDStruct* self) {
    uint32_t lastBlock  = self->BlockCount >= 1 ? self->BlockCount - 1 : self->BlockCount;
    uint8_t  capacity[] = {
        (uint8_t)((lastBlock >> 24) & 0xff),       //
        (uint8_t)((lastBlock >> 16) & 0xff),       //
        (uint8_t)((lastBlock >> 8) & 0xff),        //
        (uint8_t)((lastBlock >> 0) & 0xff),        //
        (uint8_t)((self->BlockSize >> 24) & 0xff), //
        (uint8_t)((self->BlockSize >> 16) & 0xff), //
        (uint8_t)((self->BlockSize >> 8) & 0xff),  //
        (uint8_t)((self->BlockSize >> 0) & 0xff),  //
    };
    memcpy(self->ReadBuffer, capacity, sizeof(capacity));
    if (!self->Write(self, self->ReadBuffer, sizeof(capacity))) {
        return false;
    }
    return true;
}

static bool ModeSense6(InterfaceMSDStruct* self) {
    uint8_t sense6[] = {
        0x03,
        0x00,
        0x00,
        0x00,
    };
    memcpy(self->ReadBuffer, sense6, sizeof(sense6));
    if (!self->Write(self, self->ReadBuffer, sizeof(sense6))) {
        return false;
    }
    return true;
}

static bool RequestSense(InterfaceMSDStruct* self) {
    uint8_t request_sense[] = {
        0x70, 0x00,
        0x05, // Sense Key: illegal request
        0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00,
    };
    memcpy(self->ReadBuffer, request_sense, sizeof(request_sense));
    if (!self->Write(self, self->ReadBuffer, sizeof(request_sense))) {
        return false;
    }

    return true;
}

static void TestUnitReady(InterfaceMSDStruct* self) {

    if (self->CBW.DataLength != 0) {
        if ((self->CBW.Flags & 0x80) != 0) {
            // stallEndpoint(2);
        }
        else {
            // stallEndpoint(2);
        }
    }

    self->CSW.Status = CSW_PASSED;
    self->SendCSW(self);
}

static void MemoryRead(InterfaceMSDStruct* self) {
    uint32_t readLength = MIN(self->ReadBufferSize, self->Length);

    if ((self->Addr + readLength) > self->MemorySize) {
        readLength  = 0;
        self->Stage = STAGE_ERROR;
    }

    uint32_t readResult = 0;
    if (self->DiskReadCallback)
        readResult = self->DiskReadCallback(self, (uint8_t*)self->ReadBuffer, self->Addr / self->BlockSize, readLength / self->BlockSize);

    // write data which are in RAM
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;

    if (readResult == 0 || (self->Stage != PROCESS_CBW)) {
        hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)self->ReadBuffer, 0);
        self->CSW.Status = self->Stage == STAGE_ERROR ? CSW_ERROR : CSW_FAILED;
        self->Stage      = SEND_CSW;
        return;
    }

    hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)self->ReadBuffer, readLength);

    self->Addr += readLength;
    self->Length -= readLength;
    self->CSW.DataResidue -= readLength;

    if (!self->Length) {
        self->CSW.Status = CSW_PASSED;
        self->Stage      = SEND_CSW;
    }
}

static bool InfoTransfer(InterfaceMSDStruct* self) {
    uint32_t n;

    // Logical Block Address of First Block
    n = (self->CBW.CB[2] << 24) | (self->CBW.CB[3] << 16) | (self->CBW.CB[4] << 8) | (self->CBW.CB[5] << 0);

    self->Addr = n * self->BlockSize;

    // Number of Blocks to transfer
    switch (self->CBW.CB[0]) {
        case READ10:
        case WRITE10:
        case VERIFY10:
            n = (self->CBW.CB[7] << 8) | (self->CBW.CB[8] << 0);
            break;

        case READ12:
        case WRITE12:
            n = (self->CBW.CB[6] << 24) | (self->CBW.CB[7] << 16) | (self->CBW.CB[8] << 8) | (self->CBW.CB[9] << 0);
            break;
    }

    self->Length = n * self->BlockSize;

    if (!self->CBW.DataLength) { // host requests no data
        self->CSW.Status = CSW_FAILED;
        self->SendCSW(self);
        return false;
    }

    if (self->CBW.DataLength != self->Length) {
        if ((self->CBW.Flags & 0x80) != 0) {
            // stallEndpoint(EPBULK_IN);
        }
        else {
            // stallEndpoint(EPBULK_OUT);
        }

        self->CSW.Status = CSW_FAILED;
        self->SendCSW(self);
        return false;
    }

    return true;
}

static inline void CBWDecode(InterfaceMSDStruct* self, uint32_t* buffer, uint16_t size) {
    if (size != sizeof(self->CBW))
        return;

    memcpy((uint8_t*)&self->CBW, buffer, size);

    if (self->CBW.Signature != CBW_Signature)
        return;

    self->CSW.Tag         = self->CBW.Tag;
    self->CSW.DataResidue = self->CBW.DataLength;
    if ((self->CBW.CBLength < 1) || (self->CBW.CBLength > 16)) {
        self->Fail(self);
        return;
    }

    // For custom SCSI command
    if (self->CBWDecodeCallback && self->CBWDecodeCallback(self))
        return;

    switch (self->CBW.CB[0]) {
        case TEST_UNIT_READY:
            TestUnitReady(self);
            break;
        case REQUEST_SENSE:
            RequestSense(self);
            break;
        case INQUIRY:
            InquiryRequest(self);
            break;
        case MODE_SENSE6:
            ModeSense6(self);
            break;
        case READ_FORMAT_CAPACITIES:
            ReadFormatCapacity(self);
            break;
        case READ_CAPACITY:
            ReadCapacity(self);
            break;
        case READ10:
        case READ12:
            if (InfoTransfer(self)) {
                if ((self->CBW.Flags & 0x80)) {
                    self->Stage = PROCESS_CBW;
                    MemoryRead(self);
                }
                else {
                    // stallEndpoint(2);
                    self->CSW.Status = CSW_ERROR;
                    self->SendCSW(self);
                }
            }
            break;
        case WRITE10:
        case WRITE12:
            if (InfoTransfer(self)) {
                if (!(self->CBW.Flags & 0x80)) {
                    self->Stage = PROCESS_CBW;
                }
                else {
                    // stallEndpoint(2);
                    self->CSW.Status = CSW_ERROR;
                    self->SendCSW(self);
                }
            }
            break;
        case VERIFY10:
            if (!(self->CBW.CB[1] & 0x02)) {
                self->CSW.Status = CSW_PASSED;
                self->SendCSW(self);
                break;
            }
            if (InfoTransfer(self)) {
                if (!(self->CBW.Flags & 0x80)) {
                    self->Stage = PROCESS_CBW;
                    self->MemOK = true;
                }
                else {
                    // stallEndpoint(2);
                    self->CSW.Status = CSW_ERROR;
                    self->SendCSW(self);
                }
            }
            break;
        case READ_TOC:
            if (self->CBW.CB[1] == 0x02) {
                uint8_t toc[] = {
                    0x00, 0x12, 0x01, 0x01, 0x00, 0x16, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x16, 0xAA, 0x00, 0x00, 0x01, 0x1A, 0x16,
                };
                memcpy(self->ReadBuffer, toc, sizeof(toc));
                if (!self->Write(self, self->ReadBuffer, sizeof(toc))) {
                    return;
                }
            }
            else {
                self->Fail(self);
            }
            break;
        case GET_EVENT_STATUS: {
            uint8_t status[] = {
                0x00, 0x04, // Event Descriptor Length
                0x04,       // NEA | Notification Class
                0x10,       // Supported Event Classes

                0x00, // Event Codes : NoChg
                0x02, // Media Status : Present
                0x00, // Start Slot
                0x00, // End Slots
            };
            if (self->CBW.CB[4] != 0x10) {
                status[2] = 0x00;
                status[5] = 0x00;
            }
            memcpy(self->ReadBuffer, status, sizeof(status));
            if (!self->Write(self, self->ReadBuffer, sizeof(status))) {
                return;
            }
        } break;
        case MEDIA_REMOVAL:
            self->CSW.Status = CSW_PASSED;
            self->SendCSW(self);
            break;
        default:
            self->Fail(self);
            break;
    }
}

static inline void CBWOutProcess(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    switch (self->CBW.CB[0]) {
        case WRITE10:
        case WRITE12:
            MemoryWrite(self, buffer, bytesRead);
            break;
        case VERIFY10:
            MemoryVerify(self, buffer, bytesRead);
            break;
    }
}

static inline void CBWInProcess(InterfaceMSDStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    switch (self->CBW.CB[0]) {
        case READ10:
        case READ12:
            MemoryRead(self);
            break;
    }
}

bool InterfaceMSDConstractor(InterfaceMSDStruct* self) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);
    self->Init    = Init;
    self->Fail    = Fail;
    self->SendCSW = SendCSW;
    self->Write   = Write;

    self->base.ClassRequestCallback = (ClassRequestCallbackType)ClassRequestCallback;

    self->base.InterfaceExtendDescriptorSize = 0;

    self->RxEPConfig = (EPConfigStruct) {
        .Index     = MSD_RX_EP,
        .Direct    = EPDirectOut,
        .Type      = EPTypeBulk,
        .MaxPacket = MSD_MAX_PACKET,
        .MaxBuffer = 512,
        .Buffer    = NULL,
        .Callback  = (EPCallbackType)OEPCallback,
    };

    self->TxEPConfig = (EPConfigStruct) {
        .Index     = MSD_TX_EP,
        .Direct    = EPDirectIn,
        .Type      = EPTypeBulk,
        .MaxPacket = MSD_MAX_PACKET,
        .MaxBuffer = 512,
        .Buffer    = NULL,
        .Callback  = (EPCallbackType)IEPCallback,
    };

    self->BlockCount = 2048;
    self->BlockSize  = 512;

    self->InquiryData = (uint32_t*)DefaultInquiry;
    self->InquirySize = sizeof(DefaultInquiry);
    return true;
}
