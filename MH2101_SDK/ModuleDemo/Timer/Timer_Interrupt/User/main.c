#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

void NVIC_Configuration(void);
void UART_Configuration(void);
void TIMER_Configuration(void);
void TIMM0_Mdelay(uint32_t TIMx, uint32_t delay);

uint32_t TIMM0_GetTick(uint32_t TIMx);
volatile uint32_t tick_Timer0[TIM_NUM] = {0};


int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
    
	UART_Configuration();
	TIMER_Configuration();
	NVIC_Configuration();
	
    printf("MegaHunt SCPU Timer Demo V1.0.\r\n");
    
	while(1)
	{
		TIMM0_Mdelay(TIM_0, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_0, TIMM0_GetTick(TIM_0));
		
		TIMM0_Mdelay(TIM_1, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_1, TIMM0_GetTick(TIM_1));
		
		TIMM0_Mdelay(TIM_2, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_2, TIMM0_GetTick(TIM_2));
		
		TIMM0_Mdelay(TIM_3, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_3, TIMM0_GetTick(TIM_3));
		
		TIMM0_Mdelay(TIM_4, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_4, TIMM0_GetTick(TIM_4));
		
		TIMM0_Mdelay(TIM_5, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_5, TIMM0_GetTick(TIM_5));
		
		TIMM0_Mdelay(TIM_6, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_6, TIMM0_GetTick(TIM_6));
	
		TIMM0_Mdelay(TIM_7, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_7, TIMM0_GetTick(TIM_7));	
	}
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);	
}

void TIMER_Configuration(void)
{
	TIM_InitTypeDef TIM_InitStruct;
	SYSCTRL_ClocksTypeDef clocks;
	SYSCTRL_GetClocksFreq(&clocks);
	TIM_InitStruct.TIM_Period = clocks.PCLK_Frequency/1000000 * 1000;
	
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_1;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_2;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_3;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_4;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_5;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_6;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);

	TIM_InitStruct.TIMx = TIM_7;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_1_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_2_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_3_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_4_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_5_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_6_IRQn;
 	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM0_7_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
}

uint32_t TIMM0_GetTick(uint32_t TIMx)
{
	return tick_Timer0[TIMx];
}

void TIMM0_Mdelay(uint32_t TIMx, uint32_t delay)
{
	uint32_t tick = tick_Timer0[TIMx];
	
	TIM_Cmd(TIMM0, (TIM_NumTypeDef)TIMx, ENABLE);
	if ((tick + delay) < tick_Timer0[TIMx])
	{
        while((tick_Timer0[TIMx] - delay) < tick);
	}
	else
	{
        while((tick + delay) > tick_Timer0[TIMx]);
	}
	
    TIM_Cmd(TIMM0, (TIM_NumTypeDef)TIMx, DISABLE);
}

//Retarget Printf
int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
    {
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
