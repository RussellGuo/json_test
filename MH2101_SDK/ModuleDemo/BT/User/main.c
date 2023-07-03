#include <stdio.h>
#include <errno.h>
#include "hw_config.h"
#include "bt42_hci_cmd.h"
#include "multi_timer.h"



void mcu_hardware_init(void)
{
	SYSCTRL_PLLConfig(SYSCTRL_PLL_168MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_TIMM0,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
}

void debug_pin_init(void)
{
	UART_InitTypeDef UART_InitStructure;
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);//UART0 TX/RX
    UART_StructInit(&UART_InitStructure);

    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;

    UART_Init(UART0, &UART_InitStructure);	
}

int main(void)
{
	mcu_hardware_init();
	debug_pin_init();
	printf("bt init...\n");
	bt42_bluetooth_init();//¿∂—¿≥ı ºªØ
	
	while(1)
	{

		bt42_event_process();
		multi_timer_loop();
	}
	
	return 0;
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


