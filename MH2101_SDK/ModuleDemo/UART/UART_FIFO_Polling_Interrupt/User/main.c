#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


#define UART_RECV_MODE_POLLING         1
#define UART_RECV_MODE_INTERRUPT       2
#define UART_RECV_MODE_SEL             UART_RECV_MODE_POLLING

void UART_Configuration(void);
void NVIC_Configuration(void);

uint8_t tmpRece[16] = {0};
uint32_t tmpReceHead = 0, tmpReceTail = 0;
uint32_t flagRece = 0;

#if (UART_RECV_MODE_SEL == UART_RECV_MODE_INTERRUPT)
int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);
	
    NVIC_Configuration();
	UART_Configuration();
	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);
	
	printf("MegaHunt SCPU UART Demo V1.0.\n");
	
	while (1) 
	{
		if (0 != flagRece)
		{
			UART_ITConfig(UART0, UART_IT_TX_EMPTY, ENABLE);		
		}
	}
}

#elif (UART_RECV_MODE_SEL == UART_RECV_MODE_POLLING)

int main(void)
{
	uint8_t tmp[18] = {0};
	uint32_t i = 0, j = 0;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU UART Demo V1.0.\n");
	
	while (1) 
    {
		if (UART_IsRXFIFOFull(UART0))
		{
			i = 0;
			while (UART_IsRXFIFONotEmpty(UART0))
			{
				tmp[i] = UART_ReceiveData(UART0);
				i++;
			}
			j = 0;
			while (UART_IsTXFIFONotFull(UART0) && (j <= i))
			{
				UART_SendData(UART0, tmp[j]);
				j++;
			}
		}
	}
}
#endif

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
    UART_FIFOInitTypeDef UART_FIFOInitStruct;    
    UART_StructInit(&UART_InitStructure);
    UART_FIFOStructInit(&UART_FIFOInitStruct);

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
    UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
    UART_FIFOInitStruct.FIFO_Enable = ENABLE;
    UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_1;
    UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_4_Full;
    UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_2_Chars;
    UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = ENABLE;

	UART_Init(UART0, &UART_InitStructure);	
    UART_FIFOInit(UART0, &UART_FIFOInitStruct);
    UART_FIFOCmd(UART0, ENABLE);
}

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
