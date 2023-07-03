#ifndef __USBTARGET_H__
#define __USBTARGET_H__

// Dependent header, other headers are not needed

extern void USBTargetDelayUs(uint32_t us);

extern void USBTargetEnablePhy(USBHALStruct* hal, bool isEnable);

extern void USBTargetEnableInterrupt(USBHALStruct* hal, bool isEnable);

extern void USBTargetEnableModule(USBHALStruct* hal, bool isEnable);

extern void USBTargetResetModule(USBHALStruct* hal);

#endif
