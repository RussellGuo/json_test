#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_sysctrl.h"
#include "crypt_debug.h"

void r_printf(uint32_t b, char *s)
{
    if (0 != b)
    {
        DBG_PRINT("Pass ");DBG_PRINT("%s", s);
    }
    else
    {
        DBG_PRINT("Fail ");DBG_PRINT("%s", s);
        while(1);
    }
}


void ouputRes(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i;
    uint8_t *pu8Buff = pvbuff;
    
    DBG_PRINT("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        DBG_PRINT("%02X", pu8Buff[i]);
    }
    DBG_PRINT("\n");
}


void ouputArrayHex(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i;
    uint8_t *pu8Buff = pvbuff;
    
    DBG_PRINT("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        DBG_PRINT("0x%02X, ", pu8Buff[i]);
    }
    DBG_PRINT("\n");
}


volatile uint32_t gu32Tick = 0;

const uint32_t cau32TimRefClk[2] = {4500, 4500};

void timer_init_ms(uint32_t ms)
{
    TIM_InitTypeDef s;
		SYSCTRL_ClocksTypeDef Sysctrl_Clocks;
    SYSCTRL_GetClocksFreq(&Sysctrl_Clocks);
    //SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    //SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    NVIC->ICER[0] |= BIT(TIM0_0_IRQn);
//    s.TIMx = TIM_0;
//    s.TIM_Period = cau32TimRefClk[0 != (SYSCTRL->CLK_CFG&SYSCTRL_TIM_REF_SEL_Mask)] * ms;
	
		s.TIMx = TIM_0;
//    s.TIM_Period = SYSCTRL->PCLK_1MS_VAL * ms;
	s.TIM_Period =  Sysctrl_Clocks.PCLK_Frequency/1000;//cau32TimRefClk[0];

    TIM_DeInit(TIMM0);
    TIM_Cmd(TIMM0, TIM_0, DISABLE);
    TIM_Init(TIMM0, &s);
    TIM_ITConfig(TIMM0, TIM_0, ENABLE);
    gu32Tick = 0;
    TIM_Cmd(TIMM0, TIM_0, ENABLE);
    NVIC->ICPR[0] |= BIT(TIM0_0_IRQn);
    NVIC->ISER[0] |= BIT(TIM0_0_IRQn);
}

void time_avg(TIME_MS_US *pTimeTotal, uint32_t u32Count, TIME_MS_US *pTimeAvg)
{
    assert_param(NULL != pTimeTotal);
    assert_param(NULL != pTimeAvg);
    assert_param(0 != u32Count);
    
    pTimeAvg->u32Us = (pTimeTotal->u32Us + (pTimeTotal->u32Ms % u32Count) * 1000) / u32Count;
    pTimeAvg->u32Ms = pTimeTotal->u32Ms / u32Count;
}

void time_add(TIME_MS_US *pTime1, TIME_MS_US *pTime2, TIME_MS_US *pTimeSum)
{
    assert_param(NULL != pTime1);
    assert_param(NULL != pTime2);
    assert_param(NULL != pTimeSum);
    
    pTimeSum->u32Ms = pTime1->u32Ms + pTime2->u32Ms + (pTime1->u32Us + pTime2->u32Us) / 1000;
    pTimeSum->u32Us = (pTime1->u32Us + pTime2->u32Us) % 1000;
}


/**
 * Don't use this function in a interrupt which priority is less than timer tick.
 */

uint32_t get_time(TIME_MS_US *pTime)
{
    uint32_t u32Ms, u32Left, u32Tick, u32TimMs;
    SYSCTRL_ClocksTypeDef Sysctrl_Clocks;
    assert_param(NULL != pTime);
    
    TIM_Cmd(TIMM0, TIM_0, DISABLE);
    u32Left = TIMM0->TIM[TIM_0].CurrentValue;
    u32Tick = gu32Tick;
    //TIM_Cmd(TIMM0, TIM_0, ENABLE);
    
//		u32TimMs = SYSCTRL->PCLK_1MS_VAL;
    SYSCTRL_GetClocksFreq(&Sysctrl_Clocks);
	u32TimMs = Sysctrl_Clocks.PCLK_Frequency/1000;//cau32TimRefClk[0];
    //Left count less than a tick.
    u32Left = TIMM0->TIM[TIM_0].LoadCount - u32Left;
    //Get left ms
    u32Ms = u32Left / u32TimMs;
    
    //Get left us
    pTime->u32Us = (u32Left % u32TimMs)  * 1000 / u32TimMs;

    //Get total ms(left ms + tick ms).
    pTime->u32Ms = u32Ms + TIMM0->TIM[TIM_0].LoadCount / u32TimMs * u32Tick;
    
    TIM_Cmd(TIMM0, TIM_0, ENABLE);
    return 0;
}

void timer_Verify()
{
    TIME_MS_US time[3];
		uint32_t i=0;
//    #define PIN_NUM     (6)
//    GPIOA->OEN &= ~(BIT6);
//		GPIOA->PUE |= BIT6;
//		
//			
//		GPIOA->IODR &= ~BIT(PIN_NUM);
//	//    GPIOA->IODR |= BIT(PIN_NUM);
//			for(i = 0; i<100; i++);
//		
//		timer_init_ms(1);
//		GPIOA->IODR |= BIT(PIN_NUM);
//		for(i = 0; i<100000; i++);
//		for(i = 0; i<100000; i++);
//		for(i = 0; i<100000; i++);
//		GPIOA->IODR &= ~BIT(PIN_NUM);
//		for(i = 0; i<100000; i++);
//		for(i = 0; i<100000; i++);
//		for(i = 0; i<100000; i++);
//    
//    get_time(time);    
//    DBG_PRINT("%dms%dus ", time[0].u32Ms, time[0].u32Us);
    
    #define PIN_NUM     (6)
    
		GPIO_PinRemapConfig(GPIOA, GPIO_Pin_2, GPIO_Remap_1);
		GPIOA->OEN &= ~BIT(2);
    GPIOA->BSRR |= BIT(2 + 16);
    timer_init_ms(1);
		GPIOA->BSRR |= BIT(2);
		for(i = 0; i<100000; i++);
		for(i = 0; i<100000; i++);
		for(i = 0; i<100000; i++);
		GPIOA->BSRR |= BIT(2 + 16);
    get_time(time);    
    DBG_PRINT("%dms%dus ", time[0].u32Ms, time[0].u32Us);
}


void TIM0_0_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIMM0, TIM_0);
    gu32Tick++;
}

