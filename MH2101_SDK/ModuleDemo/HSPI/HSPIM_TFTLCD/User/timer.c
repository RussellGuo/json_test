#include "mhscpu.h"
#include "timer.h"


volatile uint32_t tick_Timer = 0;

void TIMER_Configuration(void)
{
	SYSCTRL_ClocksTypeDef clocks;
	TIM_InitTypeDef TIM_InitStructure;
    
	SYSCTRL_GetClocksFreq(&clocks);	
	
	TIM_InitStructure.TIM_Period = clocks.PCLK_Frequency / 1000;    // 1ms
	TIM_InitStructure.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStructure);
	TIM_ITConfig(TIMM0, TIM_InitStructure.TIMx, ENABLE);    
}

void Delay(uint32_t Delay)
{
	uint32_t tick = tick_Timer;
    
	TIM_Cmd(TIMM0, TIM_0, ENABLE);
	if ((tick + Delay) < tick_Timer)
	{
        while((tick_Timer - Delay) < tick);
	}
	else
	{
        while((tick + Delay) > tick_Timer);
	}
    
    TIM_Cmd(TIMM0, (TIM_NumTypeDef)TIM_0, DISABLE);
}


void TIM0_0_IRQHandler(void)
{
	tick_Timer++;
	TIM_ClearITPendingBit(TIMM0, TIM_0);
}
