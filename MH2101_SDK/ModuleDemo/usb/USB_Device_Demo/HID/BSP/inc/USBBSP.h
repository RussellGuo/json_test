#ifndef __USBBSP_H__
#define __USBBSP_H__

#include "USBDevice.h"



#if CONFIG_USB_DEVICE_HID
#include "USBHID.h"
#endif // CONFIG_USB_DEVICE_HID



extern USBDeviceStruct USBDevice;

extern void USBSetup(void);
extern void USBLoop(void);
extern void USBStop(void);

#endif
