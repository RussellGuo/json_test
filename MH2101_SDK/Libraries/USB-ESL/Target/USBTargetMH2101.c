#include "USBESL.h"

inline void USBTargetDelayUs(uint32_t us) {
    SYSCTRL_ClocksTypeDef sysClock;
    SYSCTRL_GetClocksFreq(&sysClock);
    volatile uint32_t delayCount = (sysClock.CPU_Frequency / 5000000) * us;
    while (delayCount--) {}
}

inline void USBTargetEnablePhy(USBHALStruct* hal, bool isEnable) {
    if (isEnable) {
        SYSCTRL->USBPHY_CR1 &= ~BIT(0);
    }
    else {
        SYSCTRL->USBPHY_CR1 |= BIT(0);
    }
}

inline void USBTargetEnableInterrupt(USBHALStruct* hal, bool isEnable) {
    IRQn_Type irq = USB_ZOFFY_IRQ;
    if (isEnable) {
        NVIC_EnableIRQ(irq);
    }
    else {
        NVIC_DisableIRQ(irq);
    }
}

inline void USBTargetEnableModule(USBHALStruct* hal, bool isEnable) {
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_USB, (FunctionalState)isEnable);
}

inline void USBTargetResetModule(USBHALStruct* hal) {
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_USB, ENABLE);
}
