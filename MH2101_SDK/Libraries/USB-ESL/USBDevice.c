#include "USBDevice.h"

static const wchar_t DefaultLangid[2] = {0x0409, L'\0'};

// Private Functions Start
static void SetState(USBDeviceStruct* self, USBDeviceStateEnum state, USBDeviceEventEnum reson);

static uint32_t GetConfigurationDescriptorSize(USBDeviceStruct* self);
static uint8_t* GetDeviceDescriptor(USBDeviceStruct* self);
static uint8_t* GetConfigurationDescriptor(USBDeviceStruct* self);
static uint8_t* GetStringDescriptor(USBDeviceStruct* self, wchar_t* wstring);
static uint8_t* GetQualiferDescriptor(USBDeviceStruct* self);

static bool RequestGetDescriptor(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestSetAddress(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestSetConfiguration(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestGetConfiguration(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestGetInterface(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestSetInterface(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestSetFeature(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestClearFeature(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
static bool RequestGetStatus(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
// Private Functions End

static void Init(USBDeviceStruct* self, USBHALStruct* hal, uint16_t vendorID, uint16_t productID, uint16_t productRelease) {
    self->HAL = hal;

    self->VendorID       = vendorID;
    self->ProductID      = productID;
    self->ProductRelease = productRelease;

    InterfaceSetupStruct* interface = &(self->ControlInterface);
    InterfaceSetupConstractor(interface);
    interface->Init(interface);
    interface->base.Device = self;
    interface->base.Index  = 0;

    bool result = self->RegisterInterfaceEndpoint(self, &interface->base);
    util_assert(result);

    self->InterfaceCount = 0;

    self->Attributes.Reserved7 = 1;

    /* Set initial device state */
    self->IsSuspended           = true;
    self->State                 = POWERED;
    self->SelectedConfiguration = 0;
}

static bool IsConfigured(USBDeviceStruct* self) {
    /* Returns true if device is in the CONFIGURED state */
    return self->IsSuspended ? false : self->State == CONFIGURED;
}

static void SofCallback(USBHALStruct* self) {
    USBDeviceStruct* device = self->Device;
    for (int i = 0; i < device->InterfaceCount; i++) {
        USBInterfaceStruct* interface = device->Interface[i];
        if (interface->SofCallback)
            interface->SofCallback(interface);
    }
}

static void BusCallback(USBHALStruct* self, USBBusEventEnum event) {
    USBDeviceStruct* device = self->Device;
    switch (event) {
        case USBBusResetEvent:
            device->IsSuspended = false;
            SetState(device, DEFAULT, (USBDeviceEventEnum)(event | 0x10));
            break;
        case USBBusSuspendEvent:
            device->IsSuspended = true;
            SetState(device, SUSPENDED, (USBDeviceEventEnum)(event | 0x10));
            break;
        case USBBusResumeEvent:
            device->IsSuspended = false;
            SetState(device, device->GetState(device), (USBDeviceEventEnum)(event | 0x10));
            break;
    }
}

static void Start(USBDeviceStruct* self) {
    USBHALStruct* hal = self->HAL;
    hal->SofCallback  = NULL;
    for (int i = 0; i < self->InterfaceCount; i++) {
        USBInterfaceStruct* interface = self->Interface[i];
        if (!hal->SofCallback && interface->SofCallback) {
            hal->SofCallback = (HalSofCallbackType)SofCallback;
            break;
        }
    }
    hal->BusCallback = (BusCallbackType)BusCallback;
    /* Start device */
    hal->StartDevice(hal, self);
    SetState(self, POWERED, USBDeviceStartEvent);
}

static void Stop(USBDeviceStruct* self) {
    USBHALStruct* hal = self->HAL;

    /* Stop device */
    hal->StopDevice(hal);

    /* Set initial device state */
    SetState(self, POWERED, USBDeviceStopEvent);
    self->SelectedConfiguration = 0;
}

static void SetState(USBDeviceStruct* self, USBDeviceStateEnum state, USBDeviceEventEnum event) {
    if (state != SUSPENDED) {
        self->State = state;
    }

    for (int i = 0; i < self->InterfaceCount; i++) {
        USBInterfaceStruct* interface = self->Interface[i];
        if (interface->DeviceCallback)
            interface->DeviceCallback(interface, self->GetState(self), event);
    }
}

static USBDeviceStateEnum GetState(USBDeviceStruct* self) {
    return self->IsSuspended ? SUSPENDED : self->State;
}

static bool RegisterInterface(USBDeviceStruct* self, USBInterfaceStruct* interface) {
    bool result = self->RegisterInterfaceEndpoint(self, interface);
    util_assert(result);

    interface->Device = self;
    interface->Index  = self->InterfaceCount++;

    util_assert(self->InterfaceCount <= CONFIG_USB_DEVICE_MAX_INTERFACE_COUNT);

    self->Interface[interface->Index] = interface;
    return true;
}

static USBInterfaceStruct* GetInterface(USBDeviceStruct* self, uint32_t interfaceIndex) {
    if (interfaceIndex >= self->InterfaceCount) {
        return NULL;
    }
    return self->Interface[interfaceIndex];
}

static bool RegisterInterfaceEndpoint(USBDeviceStruct* self, USBInterfaceStruct* interface) {
    USBHALStruct* base = self->HAL;
    for (int i = 0; i < interface->EPCount; i++) {
        EPConfigStruct* epConfig = &interface->EPConfig[i];
        if (base->EP[epConfig->Index][epConfig->Direct].Config) {
            printf_dbg("Error: Endpoint %d %d has Contigured!\n", epConfig->Index, epConfig->Direct);
            return false;
        }
        base->EP[epConfig->Index][epConfig->Direct].Config = epConfig;
    }
    return true;
}

static bool Setup(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    USBInterfaceStruct* interface = self->GetInterface(self, transfer->Packet.wIndex & 0xFF);

    /* Process standard requests */
    if (transfer->Packet.bmRequestType.Type == STANDARD_TYPE) {
        if (interface != NULL && interface->StandardRequestCallback != NULL) {
            success = interface->StandardRequestCallback(interface, transfer);
        }
        if (success)
            return true;

        switch (transfer->Packet.bRequest) {
            case GET_STATUS:
                success = RequestGetStatus(self, transfer);
                break;
            case CLEAR_FEATURE:
                success = RequestClearFeature(self, transfer);
                break;
            case SET_FEATURE:
                success = RequestSetFeature(self, transfer);
                break;
            case SET_ADDRESS:
                success = RequestSetAddress(self, transfer);
                break;
            case GET_DESCRIPTOR:
                success = RequestGetDescriptor(self, transfer);
                break;
            case SET_DESCRIPTOR:
                /* TODO: Support is optional, not implemented here */
                break;
            case GET_CONFIGURATION:
                success = RequestGetConfiguration(self, transfer);
                break;
            case SET_CONFIGURATION:
                success = RequestSetConfiguration(self, transfer);
                break;
            case GET_INTERFACE:
                success = RequestGetInterface(self, transfer);
                break;
            case SET_INTERFACE:
                success = RequestSetInterface(self, transfer);
                break;
            default:
                break;
        }
    }
    /* Process class requests */
    else if (transfer->Packet.bmRequestType.Type == CLASS_TYPE) {
        if (interface != NULL && interface->ClassRequestCallback != NULL) {
            success = interface->ClassRequestCallback(interface, transfer);
        }
    }
    else if (transfer->Packet.bmRequestType.Type == VENDOR_TYPE) {
        if (self->VendorInterface && self->VendorInterface->ClassRequestCallback) {
            success = self->VendorInterface->ClassRequestCallback(self->VendorInterface, transfer);
        }
    }
    return success;
}

static uint32_t GetConfigurationDescriptorSize(USBDeviceStruct* self) {
    uint32_t size = CONFIGURATION_DESCRIPTOR_LENGTH;
    for (int i = 0; i < self->InterfaceCount; i++) {
        USBInterfaceStruct* interface = self->Interface[i];
        size += interface->GetInterfaceDescriptorSize(interface);
    }
    return size;
}

static uint8_t* GetDeviceDescriptor(USBDeviceStruct* self) {
    uint8_t descriptor[] = {
        DEVICE_DESCRIPTOR_LENGTH,                                /* bLength */
        DEVICE_DESCRIPTOR,                                       /* bDescriptorType */
        LSB(CONFIG_USB_VERSION),                                 /* bcdUSB (LSB) */
        MSB(CONFIG_USB_VERSION),                                 /* bcdUSB (MSB) */
        0x00,                                                    /* bDeviceClass */
        0x00,                                                    /* bDeviceSubClass */
        0x00,                                                    /* bDeviceprotocol */
        64,                                                      /* bMaxPacketSize0 */
        (uint8_t)(LSB(self->VendorID)),                          /* idVendor (LSB) */
        (uint8_t)(MSB(self->VendorID)),                          /* idVendor (MSB) */
        (uint8_t)(LSB(self->ProductID)),                         /* idProduct (LSB) */
        (uint8_t)(MSB(self->ProductID)),                         /* idProduct (MSB) */
        (uint8_t)(LSB(self->ProductRelease)),                    /* bcdDevice (LSB) */
        (uint8_t)(MSB(self->ProductRelease)),                    /* bcdDevice (MSB) */
        self->Manufacturer ? STRING_OFFSET_IMANUFACTURER : 0x00, /* iManufacturer */
        self->Product ? STRING_OFFSET_IPRODUCT : 0x00,           /* iProduct */
        self->SerialNumber ? STRING_OFFSET_ISERIAL : 0x00,       /* iSerialNumber */
        0x01                                                     /* bNumConfigurations */
    };

    memcpy(self->Descriptor.Device, descriptor, DEVICE_DESCRIPTOR_LENGTH);
    return (uint8_t*)self->Descriptor.Device;
}

static uint8_t* GetConfigurationDescriptor(USBDeviceStruct* self) {
    uint8_t* descriptor = (uint8_t*)self->Descriptor.Configuration;

    uint16_t descriptorSize = GetConfigurationDescriptorSize(self);

    util_assert(descriptorSize <= CONFIG_USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE);
    util_assert(self->MaxPower <= 500);
    descriptor[0] = CONFIGURATION_DESCRIPTOR_LENGTH;
    descriptor[1] = CONFIGURATION_DESCRIPTOR;
    descriptor[2] = LSB(descriptorSize);
    descriptor[3] = MSB(descriptorSize);
    descriptor[4] = self->InterfaceCount;
    descriptor[5] = 0x01;
    descriptor[6] = STRING_OFFSET_ICONFIGURATION;
    descriptor[7] = self->Attributes.Data;
    descriptor[8] = C_POWER(self->MaxPower);

    int offset = CONFIGURATION_DESCRIPTOR_LENGTH;
    for (int i = 0; i < self->InterfaceCount; i++) {
        USBInterfaceStruct* interface = self->Interface[i];
        offset += interface->GetInterfaceDescriptor(interface, descriptor + offset);
    }
    return descriptor;
}

static size_t GetWStringLength(const wchar_t* s) {
    size_t len = 0;
    while (s[len] != L'\0') {
        if (s[++len] == L'\0')
            return len;
        if (s[++len] == L'\0')
            return len;
        if (s[++len] == L'\0')
            return len;
        ++len;
    }
    return len;
}

static uint8_t* GetStringDescriptor(USBDeviceStruct* self, wchar_t* wstring) {
    uint8_t* descriptor = (uint8_t*)self->Descriptor.String;

    uint8_t descriptorSize = wstring == 0 ? 2 : (GetWStringLength(wstring) * 2 + 2);

    util_assert(descriptorSize <= CONFIG_USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE);
    descriptor[0] = descriptorSize;
    descriptor[1] = STRING_DESCRIPTOR;

    memcpy(descriptor + 2, wstring, descriptorSize - 2);
    return descriptor;
}

static uint8_t* GetQualiferDescriptor(USBDeviceStruct* self) {
    uint8_t descriptor[QUALIFIER_DESCRIPTOR_LENGTH] = {
        QUALIFIER_DESCRIPTOR_LENGTH, /* bLength */
        QUALIFIER_DESCRIPTOR,        /* bDescriptorType */
        LSB(CONFIG_USB_VERSION),     /* bcdUSB (LSB) */
        MSB(CONFIG_USB_VERSION),     /* bcdUSB (MSB) */
        0x00,                        /* bDeviceClass */
        0x00,                        /* bDeviceSubClass */
        0x00,                        /* bDeviceprotocol */
        64,                          /* bMaxPacketSize0 */
        0x00,                        /* bNumConfigurations */
        0x00,                        /* bReserved */
    };

    memcpy(self->Descriptor.Qualifer, descriptor, QUALIFIER_DESCRIPTOR_LENGTH);
    return (uint8_t*)self->Descriptor.Qualifer;
}

static bool RequestGetDescriptor(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;
    printf_dbg("get descr: type: %02X\n", DESCRIPTOR_TYPE(transfer->Packet.wValue));
    uint8_t* descriptor = NULL;

    switch (DESCRIPTOR_TYPE(transfer->Packet.wValue)) {
        case DEVICE_DESCRIPTOR:
            printf_dbg("device descriptor\n");
            descriptor = GetDeviceDescriptor(self);
            if ((descriptor != NULL) && (descriptor[0] == DEVICE_DESCRIPTOR_LENGTH) && (descriptor[1] == DEVICE_DESCRIPTOR)) {
                transfer->Length    = DEVICE_DESCRIPTOR_LENGTH;
                transfer->Buffer    = descriptor;
                transfer->Direction = DEVICE_TO_HOST;
                success             = true;
            }
            break;
        case CONFIGURATION_DESCRIPTOR:
            printf_dbg("configuration descriptor\n");
            descriptor = GetConfigurationDescriptor(self);
            if ((descriptor != NULL) && (descriptor[0] == CONFIGURATION_DESCRIPTOR_LENGTH) && (descriptor[1] == CONFIGURATION_DESCRIPTOR)) {
                /* Get wTotalLength */
                transfer->Length    = descriptor[2] | (descriptor[3] << 8);
                transfer->Buffer    = descriptor;
                transfer->Direction = DEVICE_TO_HOST;
                success             = true;
            }
            break;
        case STRING_DESCRIPTOR:
            printf_dbg("string descriptor\n");
            switch (DESCRIPTOR_INDEX(transfer->Packet.wValue)) {
                case STRING_OFFSET_LANGID:
                    descriptor = GetStringDescriptor(self, self->Langid);
                    break;
                case STRING_OFFSET_IMANUFACTURER:
                    descriptor = GetStringDescriptor(self, self->Manufacturer);
                    break;
                case STRING_OFFSET_IPRODUCT:
                    descriptor = GetStringDescriptor(self, self->Product);
                    break;
                case STRING_OFFSET_ISERIAL:
                    descriptor = GetStringDescriptor(self, self->SerialNumber);
                    break;
                case STRING_OFFSET_ICONFIGURATION:
                    descriptor = GetStringDescriptor(self, self->ConfigurationName);
                    break;
                default:
                    break;
            }
            if (!descriptor) {
                USBInterfaceStruct* interface = self->GetInterface(self, DESCRIPTOR_INDEX(transfer->Packet.wValue) - STRING_OFFSET_IINTERFACE);

                descriptor = GetStringDescriptor(self, interface ? interface->Name : NULL);
            }
            transfer->Length    = descriptor[0];
            transfer->Buffer    = descriptor;
            transfer->Direction = DEVICE_TO_HOST;
            success             = true;
            break;
        case QUALIFIER_DESCRIPTOR:
            printf_dbg("qualifier descriptor\n");
            descriptor          = GetQualiferDescriptor(self);
            transfer->Length    = QUALIFIER_DESCRIPTOR_LENGTH;
            transfer->Buffer    = descriptor;
            transfer->Direction = DEVICE_TO_HOST;
            success             = true;
            break;
        case INTERFACE_DESCRIPTOR:
            printf_dbg("interface descriptor\n");
        case ENDPOINT_DESCRIPTOR:
            printf_dbg("endpoint descriptor\n");
            /* TODO: Support is optional, not implemented here */
            break;
        default:
            printf_dbg("error descriptor\n");
            break;
    }

    return success;
}

static bool RequestSetAddress(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    /* Set the device address */
    self->HAL->SetAddress(self->HAL, transfer->Packet.wValue);

    SetState(self, ADDRESS, USBDeviceAddressedEvent);

    return true;
}

static bool RequestSetConfiguration(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    self->SelectedConfiguration = transfer->Packet.wValue;
    /* Set the device configuration */
    if (self->SelectedConfiguration == 0) {
        /* Not configured */
        SetState(self, ADDRESS, USBDeviceDeconfiguredEvent);
    }
    else {
        if (!self->HAL->ConfigureDevice(self->HAL))
            return false;

        SetState(self, CONFIGURED, USBDeviceConfiguredEvent);
    }

    transfer->Direction = HOST_TO_DEVICE;
    return true;
}

static bool RequestGetConfiguration(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    /* Send the device configuration */
    transfer->Buffer    = &self->SelectedConfiguration;
    transfer->Length    = sizeof(self->SelectedConfiguration);
    transfer->Direction = DEVICE_TO_HOST;
    return true;
}

static bool RequestGetInterface(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    /* Return the selected alternate setting for an interface */
    if (self->State != CONFIGURED) {
        return false;
    }

    /* Send the alternate setting */
    USBInterfaceStruct* interface = self->GetInterface(self, transfer->Packet.wIndex);

    transfer->Packet.wValue = interface ? interface->SelectedAlternate : 0;

    transfer->Buffer    = (uint8_t*)&transfer->Packet.wValue;
    transfer->Length    = sizeof(transfer->Packet.wValue);
    transfer->Direction = DEVICE_TO_HOST;
    return true;
}

static bool RequestSetInterface(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    USBInterfaceStruct* interface = self->GetInterface(self, transfer->Packet.wIndex);
    if (interface) {
        interface->SelectedAlternate = transfer->Packet.wValue;

        success = true;
    }

    return success;
}

static bool RequestSetFeature(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    USBHALStruct* hal = self->HAL;

    if (self->State != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->Packet.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->Packet.bmRequestType.Recipient) {
        case DEVICE_RECIPIENT:
            /* TODO: Remote wakeup feature not supported */
            if (transfer->Packet.wValue == DEVICE_REMOTE_WAKEUP) {}
            else if (transfer->Packet.wValue == TEST_MODE) {
                // Test mode should be set in SetupCallback
                if (!transfer->Callback) {
                    transfer->Callback = RequestSetFeature;
                    return true;
                }

                hal->SetTestMode(hal, transfer->Packet.wIndex >> 8);

                success = true;
            }
            break;
        case ENDPOINT_RECIPIENT:
            if (transfer->Packet.wValue == ENDPOINT_HALT) {
                /* TODO: We should check that the endpoint number is valid */
                hal->EndpointStall(hal, transfer->Packet.wIndex & 0xf, (transfer->Packet.wIndex & 0x80) == 0x80 ? EPDirectIn : EPDirectOut, true);
                success = true;
            }
            break;
        default:
            break;
    }

    return success;
}

static bool RequestClearFeature(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    bool success = false;

    if (self->State != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->Packet.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->Packet.bmRequestType.Recipient) {
        case ENDPOINT_RECIPIENT:
            /* TODO: We should check that the endpoint number is valid */
            if (transfer->Packet.wValue == ENDPOINT_HALT) {
                uint8_t  epIndex  = transfer->Packet.wIndex & 0xf;
                EPDirect epDirect = (transfer->Packet.wIndex & 0x80) == 0x80 ? EPDirectIn : EPDirectOut;

                EPStruct* ep = &self->HAL->EP[epIndex][epDirect];
                self->HAL->EndpointStall(self->HAL, epIndex, epDirect, false);
                if (ep->Config->Interface->StandardRequestCallback)
                    ep->Config->Interface->StandardRequestCallback(ep->Config->Interface, transfer);
                success = true;
            }
            break;
        default:
            break;
    }

    return success;
}

static bool RequestGetStatus(USBDeviceStruct* self, USBSetupTransferStruct* transfer) {
    static uint16_t status;

    bool success = false;

    if (self->State != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->Packet.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->Packet.bmRequestType.Recipient) {
        case DEVICE_RECIPIENT:
            /* TODO: Currently only supports self powered devices */
            status  = DEVICE_STATUS_SELF_POWERED;
            success = true;
            break;
        case INTERFACE_RECIPIENT:
            status  = 0;
            success = true;
            break;
        case ENDPOINT_RECIPIENT:
            if (self->HAL->GetEPStatus(self->HAL, transfer->Packet.wIndex & 0xf, (transfer->Packet.wIndex & 0x80) == 0x80 ? EPDirectIn : EPDirectOut) ==
                EPStalled) {
                status = ENDPOINT_STATUS_HALT;
            }
            else {
                status = 0;
            }
            success = true;
            break;
        default:
            break;
    }

    if (success) {
        /* Send the status */
        transfer->Buffer    = (uint8_t*)&status; /* Assumes little endian */
        transfer->Length    = sizeof(status);
        transfer->Direction = DEVICE_TO_HOST;
    }

    return success;
}

bool USBDeviceConstractor(USBDeviceStruct* self) {
    self->Init              = Init;
    self->IsConfigured      = IsConfigured;
    self->Start             = Start;
    self->Stop              = Stop;
    self->RegisterInterface = RegisterInterface;
    self->GetInterface      = GetInterface;
    self->GetState          = GetState;

    self->RegisterInterfaceEndpoint = RegisterInterfaceEndpoint;

    self->Setup = Setup;

    self->Langid = (wchar_t*)DefaultLangid;

    return true;
}
