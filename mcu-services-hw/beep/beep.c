#include "mhscpu_gpio.h"
#include "beep.h"
#include "hal_system.h"

#define BEEP_PIN_DEV    GPIOB
#define BEEP_PIN        6
#define BEEP_TMR_DEV    6 //TIM_6

static void BeepPinInit(void)
{
    GPIO_PinRemapConfig(BEEP_PIN_DEV, BIT(BEEP_PIN), GPIO_Remap_2);
}

static void BeepPinDeInit(void)
{
    GPIO_InitTypeDef gpio_conf;

    gpio_conf.GPIO_Pin = BIT(BEEP_PIN);
    gpio_conf.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_conf.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(BEEP_PIN_DEV, &gpio_conf);
    GPIO_ResetBits(BEEP_PIN_DEV, BEEP_PIN);
}

void BeepEnable(unsigned long frequency)
{
    BeepPinInit();
    if (frequency != 0) {
        s_PwmConfig(0, BEEP_TMR_DEV, frequency, 8, 16);
    }
    s_PwmOnOff(0, BEEP_TMR_DEV, 1);
}

void BeepDisable(void)
{
    s_PwmOnOff(0, BEEP_TMR_DEV, 0);
    BeepPinDeInit();
}

int BeepSuspend(void)
{
    BeepDisable();
    return 0;
}

int BeepResume(void)
{
    return 0;
}

void BEEP_ON(void)
{
    BeepEnable(BEEP_DEFAULT_FREQ);
}
void BEEP_OFF(void)
{
    BeepDisable();
}

