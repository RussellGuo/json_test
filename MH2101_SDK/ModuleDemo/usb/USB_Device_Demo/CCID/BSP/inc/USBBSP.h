#ifndef __USBBSP_H__
#define __USBBSP_H__

#include "USBDevice.h"




#if CONFIG_USB_DEVICE_CCID
#include "USBCCID.h"
#endif // CONFIG_USB_DEVICE_CCID


extern USBDeviceStruct USBDevice;

extern void USBSetup(void);
extern void USBLoop(void);
extern void USBStop(void);

#endif
