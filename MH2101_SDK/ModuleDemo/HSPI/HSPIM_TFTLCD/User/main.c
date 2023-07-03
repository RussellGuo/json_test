#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "timer.h"
#include "lcd.h"

void UART_Configuration(void);
void NVIC_Configuration(void);
HSPIM_TypeDef *HSPIx = HSPIM0;
int main(void)
{
//	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
//	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_HSPI | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1 | SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	UART_Configuration();
	NVIC_Configuration();
	TIMER_Configuration();
	TFT_LCD_Init();
	
	printf(">>> MHSCPU HSPI_LCD Demo v1.0\n");	

    while(1)
	{
		TFT_LCD_Clear(TFT_LCD_COLOR_WHITE);	
		TFT_LCD_ShowString(50, 150, "Show Image Directly", TFT_LCD_COLOR_WHITE, TFT_LCD_COLOR_BLUE);   
		TFT_LCD_ShowImageDirect();  	
		Delay(1000); 

		TFT_LCD_Clear(TFT_LCD_COLOR_WHITE);     
		TFT_LCD_ShowString(50, 150, "Show Image DMA", TFT_LCD_COLOR_WHITE, TFT_LCD_COLOR_BLUE);	
		TFT_LCD_ShowImageDMA();  
		Delay(1000);
	}
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
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

int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);

	return ch;
}


int fputc(int c, FILE *f)
{
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while(1);
}
#endif

