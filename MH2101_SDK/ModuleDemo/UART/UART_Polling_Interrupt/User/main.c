#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


#define UART_RECV_MODE_POLLING         1
#define UART_RECV_MODE_INTERRUPT       2
#define UART_RECV_MODE_SEL             UART_RECV_MODE_INTERRUPT

uint8_t tmpRece = 0;
uint32_t flagRece = 0;

void NVIC_Configuration(void);
void UART_Configuration(void);


#if (UART_RECV_MODE_SEL == UART_RECV_MODE_INTERRUPT)
int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

	UART_Configuration();
	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);	
	NVIC_Configuration();
 	printf("MegaHunt SCPU UART Demo V1.0.\n");      
    while (1);

}

#elif (UART_RECV_MODE_SEL == UART_RECV_MODE_POLLING)

int main(void)
{
    uint8_t tmp;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);

	UART_Configuration();
 	printf("MegaHunt SCPU UART Demo V1.0.\n");
    while (1)
    {
 		if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
 		{
 			tmp = UART_ReceiveData(UART0);
 			UART_SendData(UART0, tmp);
 		}
    }
}
#endif


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 9600;
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
