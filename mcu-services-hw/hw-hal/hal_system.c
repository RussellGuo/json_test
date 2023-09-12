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

void s_PwmConfig(unsigned char ftm_id,unsigned char ch_id,unsigned long CLK,unsigned short hi,unsigned short pero)
{
	TIMM0->TIM[ch_id].ControlReg =0;
	TIMM0->TIM[ch_id].LoadCount =get_apb_frequency()*hi/CLK/pero -1;
	TIMM0->TIM_ReloadCount[ch_id] =get_apb_frequency()*(pero-hi)/CLK/pero -1;
	TIMM0->TIM[ch_id].EOI;
	TIMM0->TIM[ch_id].ControlReg =BIT_TIMM_CR_IEDIS|BIT_TIMM_CR_PWM|BIT_TIMM_CR_US;
	(void)ftm_id;
}

void s_PwmOnOff(unsigned char ftm_id,unsigned char ch_id,unsigned char OnOff)
{
	if(OnOff)
		TIMM0->TIM[ch_id].ControlReg |=BIT_TIMM_CR_PWM|BIT_TIMM_CR_EN;
	else
	{
		TIMM0->TIM[ch_id].ControlReg &=~(BIT_TIMM_CR_PWM|BIT_TIMM_CR_EN);
	}
    (void)ftm_id;
}
