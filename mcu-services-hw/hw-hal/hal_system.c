#include "hal_system.h"
#include "mhscpu_sysctrl.h"

void delaycnt(unsigned int cnt)
{
    while(cnt--)
    {
        __asm ("nop");
    }
}

void DelayMs(unsigned int ms)
{
    while(ms--) delaycnt(CNT_DELAYMS);
}

void DelayUs(unsigned int us)
{
    while(us--) delaycnt(CNT_DELAYUS);
}

unsigned long get_system_frequency(void)
{
    SYSCTRL_ClocksTypeDef sysclk;

    SYSCTRL_GetClocksFreq(&sysclk);
    return sysclk.PLL_Frequency;
}

unsigned long get_ahb_frequency(void)
{
    SYSCTRL_ClocksTypeDef ahbclk;

    SYSCTRL_GetClocksFreq(&ahbclk);
    return ahbclk.HCLK_Frequency;
}

unsigned long get_apb_frequency(void)
{
    SYSCTRL_ClocksTypeDef apbclk;

    SYSCTRL_GetClocksFreq(&apbclk);
    return apbclk.PCLK_Frequency;
}
