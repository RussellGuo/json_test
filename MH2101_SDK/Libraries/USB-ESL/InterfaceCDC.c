#include "InterfaceCDC.h"
#include "USBDevice.h"

static const uint8_t DefaultCDCExtendDescriptor[CDC_EXTEND_DESCRIPTOR_SIZE] = {
    // CDC Header Functional Descriptor, CDC Spec 5.2.3.1, Table 26
    0x05, // bFunctionLength
    0x24, // bDescriptorType
    0x00, // bDescriptorSubtype
    0x10, // bcdCDC
    0x01, // bcdCDC

    // Call Management Functional Descriptor, CDC Spec 5.2.3.2, Table 27
    0x05, // bFunctionLength
    0x24, // bDescriptorType
    0x01, // bDescriptorSubtype
    0x03, // bmCapabilities
    0x01, // bDataInterface

    // Abstract Control Management Functional Descriptor, CDC Spec 5.2.3.3, Table 28
    0x04, // bFunctionLength
    0x24, // bDescriptorType
    0x02, // bDescriptorSubtype
    0x06, // bmCapabilities

    // Union Functional Descriptor, CDC Spec 5.2.3.8, Table 33
    0x05, // bFunctionLength
    0x24, // bDescriptorType
    0x06, // bDescriptorSubtype
    0x00, // bMasterInterface
    0x01, // bSlaveInterface0
};

static void Init(InterfaceCDCStruct* self) {
    USBInterfaceStruct* base = (USBInterfaceStruct*)self;
    base->Init(base, 0x02, 0x02, 0x00, self->TxEPConfig.Index ? 1 : 0, self->InterfaceEPConfig);
    base->AssociateInterfaceCount = 2;

    memcpy(self->ExtendDescriptorData, DefaultCDCExtendDescriptor, CDC_EXTEND_DESCRIPTOR_SIZE);
    base->InterfaceExtendDescriptorSize = CDC_EXTEND_DESCRIPTOR_SIZE;
    base->InterfaceExtendDescriptor     = (uint8_t*)self->ExtendDescriptorData;

    self->DTERate    = 115200;
    self->CharFormat = 0;
    self->ParityType = 0;
    self->DataBits   = 8;

    base->ClassRequestCallback = (ClassRequestCallbackType)self->ClassRequestCallback;
}

static bool ClassRequestCallback(InterfaceCDCStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;
    switch (transfer->Packet.bRequest) {
        case CDC_GET_LINE_CODING:
            transfer->Length    = CDC_LINE_CODING_SIZE;
            transfer->Buffer    = (uint8_t*)self->LineCodingData;
            transfer->Direction = DEVICE_TO_HOST;
            success             = true;
            break;
        case CDC_SET_LINE_CODING:
            // Setup request
            if (transfer->Buffer == NULL) {
                transfer->Length    = CDC_LINE_CODING_SIZE;
                transfer->Buffer    = (uint8_t*)self->LineCodingData;
                transfer->Direction = HOST_TO_DEVICE;
                success             = true;
                break;
            }
            // Data phase completed
            if (self->SetLineCodingCallback) {
                self->SetLineCodingCallback(self, self->DTERate, self->CharFormat, self->ParityType, self->DataBits);
            }
            success = true;
            break;
        case CDC_SET_CONTROL_LINE_STATE: {
            self->Serial->StartProbe(self->Serial);
            success = true;
        } break;
        default:
            break;
    }
    return success;
}

bool InterfaceCDCConstractor(InterfaceCDCStruct* self, InterfaceSerialStruct* serial) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);
    self->Init = Init;

    self->ClassRequestCallback = ClassRequestCallback;

    self->Serial = serial;

    self->TxEPConfig = (EPConfigStruct) {
        .Index      = 0,
        .Direct     = EPDirectIn,
        .Type       = EPTypeInterrupt,
        .Interval   = 0x80,
        .HSInterval = 0x0B,
        .MaxPacket  = 16,
        .MaxBuffer  = 0,
        .Buffer     = NULL,
        .Callback   = NULL,
    };

    return true;
}
