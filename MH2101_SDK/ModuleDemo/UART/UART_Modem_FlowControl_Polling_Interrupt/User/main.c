#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "ring_buffer.h"

//#define POLLING    1
#define INTERRUPT  1

void UART_Configuration(void);
void NVIC_Configuration(void);

RingBufferTypeDef ringbuf;
uint8_t tmpRece = 0;
uint32_t flagRece = 0;

uint8_t u8GetDate[1024] = {0};
uint32_t u32GetDateIndex = 0;
uint32_t u32GetDateNum = 4;


int main(void)
{
#ifdef INTERRUPT
    int32_t tmpSend;
#endif
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);
	
	NVIC_Configuration();
    UART_Configuration();
    
	printf("MegaHunt SCPU UART Demo V1.0.\n");

#ifdef POLLING
	while (1) 
	{
		u32GetDateIndex = 0;
		if(u32GetDateIndex < u32GetDateNum)
			UART_SetRTS(UART0);
		do{
			if(UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
			{
				u8GetDate[u32GetDateIndex] = UART_ReceiveData(UART0);
				u32GetDateIndex ++;
			}
		}while(u32GetDateIndex < u32GetDateNum);
		UART_ResetRTS(UART0);
		
		u32GetDateIndex = 0;
		while((u32GetDateIndex < u32GetDateNum))
		{
				while(!UART_IsTXEmpty(UART0));
				UART_SendData(UART0, u8GetDate[u32GetDateIndex]);
				u8GetDate[u32GetDateIndex] = 0;
				u32GetDateIndex++;
		}
	}
#endif

#ifdef INTERRUPT
    UART_SetRTS(UART0);
    while(1) 
    {
        if(ringbuf.count >= 1)
        {	
            while(ringbuf.count > 0)
            {
                if(UART0->MSR & UART_MODEM_STATUS_CTS)
                {
                    tmpSend = ringBufferGet(&ringbuf);
                    while(!UART_IsTXEmpty(UART0));
                    UART_SendData(UART0, (uint8_t)tmpSend);
                }
            }
            UART_SetRTS(UART0);
        }
    }
#endif    
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	UART_FIFOInitTypeDef UART_FIFOInitStruct;

	UART_StructInit(&UART_InitStructure);
	UART_FIFOStructInit(&UART_FIFOInitStruct);
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3, GPIO_Remap_0);
	
    UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_FIFOInitStruct.FIFO_Enable = ENABLE;
	UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_0;
	UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = ENABLE;
	
    UART_FIFOInit(UART0, &UART_FIFOInitStruct);
    UART_Init(UART0, &UART_InitStructure);
    UART_FIFOCmd(UART0, ENABLE);
	UART_AutoFlowCtrlCmd(UART0, ENABLE);
 
#ifdef INTERRUPT
    UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);
#endif
}

void NVIC_Configuration(void)
{
#ifdef INTERRUPT
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
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
