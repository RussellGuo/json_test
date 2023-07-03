#ifndef __USBINTERFACETYPES_H__
#define __USBINTERFACETYPES_H__

// Dependent header, other headers are not needed

typedef struct InterfaceDescriptorStruct_s InterfaceDescriptorStruct;
typedef struct EndpointDescriptorStruct_s  EndpointDescriptorStruct;
typedef struct USBInterfaceStruct_s        USBInterfaceStruct;

extern bool USBInterfaceConstractor(USBInterfaceStruct* self);

#endif
