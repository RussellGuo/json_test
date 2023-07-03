/************************ (C) COPYRIGHT Megahuntmicro ********************************************************
 * @file                : main.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 9-November-2019
 *************************************************************************************************************/
#include <stdio.h>
#include "mhscpu.h"
#include "systick.h"

void UART_Configuration(void);

extern void LCD_Test(void);

HSPIM_TypeDef *HSPIx = HSPIM0;

int main(void)
{
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1 | SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	SYSTICK_Init();	
	UART_Configuration();	
	
	printf(">>>>> MH2101 HSPI LCD demo v1.0 \n");

	LCD_Test();
	
    while(1);

}

void UART_Configuration(void)
{
	UART_InitTypeDef uart;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	uart.UART_BaudRate = 115200;
	uart.UART_WordLength = UART_WordLength_8b;
	uart.UART_StopBits = UART_StopBits_1;
	uart.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &uart);
}


int SER_PutChar(int ch)
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

