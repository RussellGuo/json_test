#ifndef __USBDEVICE_H__
#define __USBDEVICE_H__

#include "InterfaceSetup.h"

struct USBDeviceStruct_s {
    void (*Init)(USBDeviceStruct* self, USBHALStruct* hal, uint16_t vendorID, uint16_t productID, uint16_t productRelease);
    bool (*IsConfigured)(USBDeviceStruct* self);
    void (*Start)(USBDeviceStruct* self);
    void (*Stop)(USBDeviceStruct* self);
    bool (*RegisterInterface)(USBDeviceStruct* self, USBInterfaceStruct* interface);
    bool (*RegisterInterfaceEndpoint)(USBDeviceStruct* self, USBInterfaceStruct* interface);
    USBInterfaceStruct* (*GetInterface)(USBDeviceStruct* self, uint32_t interfaceIndex);
    USBDeviceStateEnum (*GetState)(USBDeviceStruct* self);

    bool (*Setup)(USBDeviceStruct* self, USBSetupTransferStruct* transfer);

    uint16_t VendorID;
    uint16_t ProductID;
    uint16_t ProductRelease;
    uint16_t MaxPower;
    wchar_t* Langid;
    wchar_t* Manufacturer;
    wchar_t* Product;
    wchar_t* SerialNumber;
    wchar_t* ConfigurationName;

    USBAttributesUnion Attributes;
    USBDescriptorUnion Descriptor;

    // private Member Start
    USBHALStruct* HAL;

    InterfaceSetupStruct ControlInterface;
    USBInterfaceStruct*  VendorInterface;

    USBInterfaceStruct* Interface[CONFIG_USB_DEVICE_MAX_INTERFACE_COUNT];

    uint32_t InterfaceCount;

    USBDeviceStateEnum State;

    // Meano TODO: Need struct to switch Configuration or Interface's Alternate
    uint8_t SelectedConfiguration;
    bool    IsSuspended;
    // Private Member End
};

#endif
