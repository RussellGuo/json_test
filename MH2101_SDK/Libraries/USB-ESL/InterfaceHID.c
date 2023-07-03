#include "InterfaceHID.h"
#include "USBDevice.h"

static const uint8_t DefaultHIDVendorReportDescriptor[] __ALIGNED(4) = {
    USAGE_PAGE(2),      DATA_SHORT(0xFFAB), //
    USAGE(2),           DATA_SHORT(0x0200), //
    COLLECTION(1),      0x01,               // Collection (Application)

    REPORT_SIZE(1),     0x08, // 8 bits
    LOGICAL_MINIMUM(1), 0x80, //
    LOGICAL_MAXIMUM(1), 0x7F, //

    REPORT_COUNT(2),    DATA_SHORT(HID_VENDOR_REPORT_SIZE), // Input Report
    USAGE(1),           0x01,                               //
    INPUT(1),           0x02,                               // Data, Var, Abs

    REPORT_COUNT(2),    DATA_SHORT(HID_VENDOR_REPORT_SIZE), // Output Report
    USAGE(1),           0x02,                               //
    OUTPUT(1),          0x02,                               // Data, Var, Abs

    END_COLLECTION(0),
};

static void Init(InterfaceHIDStruct* self) {
    USBInterfaceStruct* base = (USBInterfaceStruct*)self;

    ((uint8_t*)self->HIDDescriptorData)[7] = LSB(self->ReportDescriptorSize);
    ((uint8_t*)self->HIDDescriptorData)[8] = MSB(self->ReportDescriptorSize);

    base->InterfaceExtendDescriptorSize = HID_DESCRIPTOR_SIZE;
    base->InterfaceExtendDescriptor     = (uint8_t*)self->HIDDescriptorData;

    self->RxEPConfig.MaxBuffer = self->OutputReportSize;
    self->RxEPConfig.Buffer    = self->OutputReportData;

    self->IsInputPending  = false;
    self->IsOutputUpdated = false;

    uint8_t epCount = 2;

    EPConfigStruct* epConfig = self->InterfaceEPConfig;

    if (!self->IsOutputEPEnabled) {
        epCount--;
        epConfig++;
    }
    if (!self->IsInputEPEnabled) {
        epCount--;
    }

    base->Init(base, HID_CLASS, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE, epCount, epConfig);
}

static bool PutInputReport(InterfaceHIDStruct* self, uint8_t* inputReport, uint32_t inputSize) {
    USBDeviceStruct* device = self->base.Device;
    if (!device->IsConfigured(device))
        return false;

    if (self->IsInputPending)
        return false;

    inputSize = MIN(self->InputReportSize, inputSize);
    memcpy(self->InputReportData, inputReport, inputSize);
    self->IsInputPending = true;

    if (!self->IsInputEPEnabled)
        return true;

    USBHALStruct* hal = self->base.Device->HAL;
    hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->InputReportData, self->InputReportSize);
    return true;
}

static bool GetOutputReport(InterfaceHIDStruct* self, uint8_t* outputReport, uint32_t outputSize) {
    outputSize = MIN(self->OutputReportSize, outputSize);
    memcpy(outputReport, self->OutputReportData, self->OutputReportSize);

    if (!self->IsOutputUpdated)
        return false;

    self->IsOutputUpdated = false;
    return true;
}

static bool InputCallback(InterfaceHIDStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    self->IsInputPending = false;
    if (self->GetReportCallback && self->GetReportCallback(self, (uint8_t*)self->InputReportData, self->InputReportSize)) {
        self->IsInputPending = true;

        if (!self->IsInputEPEnabled)
            return true;

        USBHALStruct* hal = self->base.Device->HAL;
        hal->EndpointStartWrite(hal, self->TxEPConfig.Index, self->InputReportData, self->InputReportSize);
    }
    return true;
}

static bool OutputCallback(InterfaceHIDStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    if (!self->IsOutputUpdated) {
        self->IsOutputUpdated = true;
    }

    if (self->SetReportCallback)
        self->SetReportCallback(self, (uint8_t*)self->OutputReportData, self->OutputReportSize);

    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    hal->EndpointStartRead(hal, self->RxEPConfig.Index, self->OutputReportData, self->OutputReportSize);
    return true;
}

static bool StandardRequestCallback(InterfaceHIDStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    switch (transfer->Packet.bRequest) {
        case GET_DESCRIPTOR:
            switch (DESCRIPTOR_TYPE(transfer->Packet.wValue)) {
                case REPORT_DESCRIPTOR:
                    if (self->ReportDescriptorSize > 0) {
                        transfer->Length    = self->ReportDescriptorSize;
                        transfer->Buffer    = (uint8_t*)self->ReportDescriptorData;
                        transfer->Direction = DEVICE_TO_HOST;
                        success             = true;
                    }
                    break;
                case HID_DESCRIPTOR:
                    transfer->Length    = HID_DESCRIPTOR_SIZE;
                    transfer->Buffer    = (uint8_t*)self->HIDDescriptorData;
                    transfer->Direction = DEVICE_TO_HOST;
                    success             = true;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return success;
}

static bool ClassRequestCallback(InterfaceHIDStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    uint8_t* deviceReportBuffer = (uint8_t*)self->base.Device->Descriptor.Report;

    switch (transfer->Packet.bRequest) {
        case SET_REPORT:
            success = true;
            // Setup request
            if (transfer->Buffer == NULL) {
                transfer->Length    = self->OutputReportSize;
                transfer->Buffer    = deviceReportBuffer;
                transfer->Direction = HOST_TO_DEVICE;
                break;
            }

            // Data phase completed
            memcpy(self->OutputReportData, deviceReportBuffer, self->OutputReportSize);
            if (self->SetReportCallback && self->SetReportCallback(self, deviceReportBuffer, self->OutputReportSize))
                self->IsOutputUpdated = false;
            else
                self->IsOutputUpdated = true;

            break;
        case GET_REPORT:
            if (transfer->Packet.wLength != self->InputReportSize)
                break;

            if (self->GetReportCallback && self->GetReportCallback(self, (uint8_t*)self->InputReportData, self->InputReportSize))
                self->IsInputPending = true;

            if (!self->IsInputPending)
                break;

            memcpy(deviceReportBuffer, self->InputReportData, self->InputReportSize);
            transfer->Length    = self->InputReportSize;
            transfer->Buffer    = deviceReportBuffer;
            transfer->Direction = DEVICE_TO_HOST;

            self->IsInputPending = false;

            success = true;
            break;
        case GET_IDLE:
            transfer->Length    = transfer->Packet.wLength;
            transfer->Buffer    = (uint8_t*)&self->Idle;
            transfer->Direction = DEVICE_TO_HOST;
            success             = true;
            break;
        case SET_IDLE:
            self->Idle          = transfer->Packet.wValue;
            transfer->Direction = HOST_TO_DEVICE;
            success             = true;
            if (self->SetIdleCallback)
                self->SetIdleCallback(self);
            break;
        case GET_PROTOCOL:
            break;
        case SET_PROTOCOL:
            break;
        default:
            break;
    }
    return success;
}

bool InterfaceHIDConstractor(InterfaceHIDStruct* self) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);
    self->Init            = Init;
    self->PutInputReport  = PutInputReport;
    self->GetOutputReport = GetOutputReport;

    self->base.StandardRequestCallback = (StandardRequestCallbackType)StandardRequestCallback;
    self->base.ClassRequestCallback    = (ClassRequestCallbackType)ClassRequestCallback;

    // Set default hid descriptor
    uint8_t hidDescriptor[HID_DESCRIPTOR_SIZE] = {
        HID_DESCRIPTOR_SIZE, // bLength
        HID_DESCRIPTOR,      // bDescriptorType
        LSB(HID_VERSION),    // bcdHID (LSB)
        MSB(HID_VERSION),    // bcdHID (MSB)
        0x00,                // bCountryCode
        0x01,                // bNumDescriptors
        REPORT_DESCRIPTOR,   // bDescriptorType
        0x00,                // wDescriptorLength (LSB)
        0x00,                // wDescriptorLength (MSB)
    };

    memcpy(self->HIDDescriptorData, hidDescriptor, HID_DESCRIPTOR_SIZE);

    // Set default hid report descriptor
    self->ReportDescriptorData = (uint32_t*)DefaultHIDVendorReportDescriptor;
    self->ReportDescriptorSize = sizeof(DefaultHIDVendorReportDescriptor);

    self->IsInputEPEnabled  = true;
    self->IsOutputEPEnabled = false;

    self->TxEPConfig = (EPConfigStruct) {
        .Index      = HID_TX_EP,
        .Direct     = EPDirectIn,
        .Type       = EPTypeInterrupt,
        .Interval   = 1,
        .HSInterval = 4,
        .MaxPacket  = HID_MAX_PACKET,
        .MaxBuffer  = HID_MAX_PACKET,
        .Buffer     = NULL,
        .Callback   = (EPCallbackType)InputCallback,
    };

    self->RxEPConfig = (EPConfigStruct) {
        .Index      = HID_RX_EP,
        .Direct     = EPDirectOut,
        .Type       = EPTypeInterrupt,
        .Interval   = 1,
        .HSInterval = 4,
        .MaxPacket  = HID_MAX_PACKET,
        .MaxBuffer  = HID_MAX_PACKET,
        .Buffer     = NULL,
        .Callback   = (EPCallbackType)OutputCallback,
    };

    return true;
}
