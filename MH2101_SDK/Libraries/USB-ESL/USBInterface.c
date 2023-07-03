#include "USBInterface.h"
#include "USBDevice.h"

static void Init(USBInterfaceStruct* self, uint8_t iClass, uint8_t iSubClass, uint8_t iProtocol, uint8_t epCount, EPConfigStruct* epConfig) {
    self->Class    = iClass;
    self->SubClass = iSubClass;
    self->Protocol = iProtocol;
    self->EPCount  = epCount;
    self->EPConfig = epConfig;
    for (int i = 0; i < epCount; i++) {
        self->EPConfig[i].Interface = self;
    }
}

static uint32_t GetInterfaceDescriptorSize(USBInterfaceStruct* self) {
    int descriptorSize = (self->AssociateInterfaceCount ? 8 : 0) + (self->AlternateCount + 1) * 9 + self->InterfaceExtendDescriptorSize;
    for (int i = 0; i < self->EPCount; i++) {
        EPConfigStruct* epConfig = &self->EPConfig[i];
        descriptorSize += ENDPOINT_DESCRIPTOR_LENGTH + (epConfig->Type == EPTypeISO ? 2 : 0) + (epConfig->ExtendDescriptor ? epConfig->ExtendDescriptor[0] : 0);
    }
    return descriptorSize;
}

static uint32_t GetInterfaceDescriptor(USBInterfaceStruct* self, uint8_t* descriptorBuffer) {
    uint32_t offset = 0;
    if (self->AssociateInterfaceCount) {
        descriptorBuffer[offset + 0] = 0x08;                          // bLength
        descriptorBuffer[offset + 1] = 0x0b;                          // bDescriptorType
        descriptorBuffer[offset + 2] = self->Index;                   // bFirstInterface
        descriptorBuffer[offset + 3] = self->AssociateInterfaceCount; // bInterfaceCount
        descriptorBuffer[offset + 4] = self->Class;                   // bFunctionClass
        descriptorBuffer[offset + 5] = self->SubClass;                // bFunctionSubClass
        descriptorBuffer[offset + 6] = self->Protocol;                // bFunctionProtocol

        descriptorBuffer[offset + 7] = self->Name ? self->Index + STRING_OFFSET_IINTERFACE : 0; // iFunction
        offset += 8;
    }

    InterfaceDescriptorStruct* interfaceDescriptor;

    for (int i = 0; i < (self->AlternateCount + 1); i++) {
        interfaceDescriptor = (InterfaceDescriptorStruct*)(descriptorBuffer + offset);

        interfaceDescriptor->bLength            = INTERFACE_DESCRIPTOR_LENGTH;
        interfaceDescriptor->bDescriptorType    = INTERFACE_DESCRIPTOR;
        interfaceDescriptor->bInterfaceNumber   = self->Index;
        interfaceDescriptor->bAlternateSetting  = i;
        interfaceDescriptor->bNumEndpoints      = i == self->AlternateCount ? self->EPCount : 0;
        interfaceDescriptor->bInterfaceClass    = self->Class;
        interfaceDescriptor->bInterfaceSubClass = self->SubClass;
        interfaceDescriptor->bInterfaceProtocol = self->Protocol;
        interfaceDescriptor->iInterface         = self->Name ? self->Index + STRING_OFFSET_IINTERFACE : 0;

        offset += INTERFACE_DESCRIPTOR_LENGTH;
    }

    if (self->InterfaceExtendDescriptorSize != 0) {
        memcpy(descriptorBuffer + offset, self->InterfaceExtendDescriptor, self->InterfaceExtendDescriptorSize);
        offset += self->InterfaceExtendDescriptorSize;
    }

    for (int i = 0; i < self->EPCount; i++) {
        EPConfigStruct* epConfig = &self->EPConfig[i];

        EndpointDescriptorStruct* epDescriptor = (EndpointDescriptorStruct*)(descriptorBuffer + offset);

        epDescriptor->bLength           = ENDPOINT_DESCRIPTOR_LENGTH;
        epDescriptor->bDescriptorType   = ENDPOINT_DESCRIPTOR;
        epDescriptor->bEndpointAddress  = epConfig->Index | (epConfig->Direct << 7);
        epDescriptor->bmAttributes      = epConfig->Type;
        epDescriptor->wMaxPacketSizeLSB = LSB(epConfig->MaxPacket);
        epDescriptor->wMaxPacketSizeMSB = MSB(epConfig->MaxPacket);
        epDescriptor->bInterval         = self->Device->HAL->Speed == USBSpeedHigh ? epConfig->HSInterval : epConfig->Interval;
        offset += ENDPOINT_DESCRIPTOR_LENGTH;

        if (epConfig->Type == EPTypeISO) {
            epDescriptor->bRefresh      = 0x00;
            epDescriptor->bSynchAddress = 0x00;
            epDescriptor->bLength += 2;
            offset += 2;
        }

        if (epConfig->ExtendDescriptor) {
            memcpy(descriptorBuffer + offset, epConfig->ExtendDescriptor, epConfig->ExtendDescriptor[0]);
            offset += epConfig->ExtendDescriptor[0];
        }
    }
    return offset;
}

bool USBInterfaceConstractor(USBInterfaceStruct* self) {
    self->Init                       = Init;
    self->GetInterfaceDescriptorSize = GetInterfaceDescriptorSize;
    self->GetInterfaceDescriptor     = GetInterfaceDescriptor;

    self->Name = NULL;
    return true;
}
