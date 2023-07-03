#ifndef __USBINTERFACE_H__
#define __USBINTERFACE_H__

#include "USBESL.h"

typedef bool (*EPCallbackType)(USBInterfaceStruct* interface, uint32_t* buffer, uint32_t bytesTransfer);
typedef bool (*DeviceCallbackType)(USBInterfaceStruct* self, USBDeviceStateEnum state, USBDeviceEventEnum event);
typedef bool (*StandardRequestCallbackType)(USBInterfaceStruct* self, USBSetupTransferStruct* transfer);
typedef bool (*ClassRequestCallbackType)(USBInterfaceStruct* self, USBSetupTransferStruct* transfer);
typedef bool (*SofCallbackType)(USBInterfaceStruct* self);

struct InterfaceDescriptorStruct_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
};

struct EndpointDescriptorStruct_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSizeLSB;
    uint8_t wMaxPacketSizeMSB;
    uint8_t bInterval;
    uint8_t bRefresh;
    uint8_t bSynchAddress;
};

struct USBInterfaceStruct_s {
    void (*Init)(USBInterfaceStruct* self, uint8_t class, uint8_t subClass, uint8_t protocol, uint8_t epCount, EPConfigStruct* epConfig);
    uint32_t (*GetInterfaceDescriptorSize)(USBInterfaceStruct* self);
    uint32_t (*GetInterfaceDescriptor)(USBInterfaceStruct* self, uint8_t* descriptorBuffer);

    USBDeviceStruct* Device;
    EPConfigStruct*  EPConfig;
    uint8_t          EPCount;

    uint8_t Index;
    uint8_t Class;
    uint8_t SubClass;
    uint8_t Protocol;

    uint8_t AssociateInterfaceCount;
    uint8_t AlternateCount;
    uint8_t SelectedAlternate;

    uint32_t InterfaceExtendDescriptorSize;
    uint8_t* InterfaceExtendDescriptor;

    wchar_t* Name;

    // Extends class callback
    DeviceCallbackType          DeviceCallback;
    StandardRequestCallbackType StandardRequestCallback;
    ClassRequestCallbackType    ClassRequestCallback;
    SofCallbackType             SofCallback;
};

#endif
