#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_gpio.h"

#define DATA_SIZE 									256		
#define HSPIx										HSPIS0
#define UARTx										UART0
																
void HSPIS_Configuration(void);
void UART_Configuration(void);
void NVIC_Configuration(void);
void DataPrint(uint8_t *Buff, uint32_t Len);
int DataCheck(uint8_t *Src, uint8_t *Dst, uint32_t Len);
uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];
uint32_t SendIndex, RecvIndex, RecvFlag;
																
																
int  main (void)
{
	uint32_t i;
	
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
		
	UART_Configuration();
	
	printf("MH2101 HSPI Slave Interrput Tx Rx Demo v1.0\n");
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i;
		RecvBuff[i] = 0;
	}	

	NVIC_Configuration();
	
	HSPIS_Configuration();
	
	while(1)
	{		
		if (RecvFlag)
		{	
			RecvFlag = 0;
		
			if (DataCheck(SendBuff, RecvBuff, DATA_SIZE))
			{
				DataPrint(RecvBuff, DATA_SIZE);
				while(1);
			}
		}
	}
}


void HSPI0_IRQHandler(void)
{
	if (HSPIS_GetITStatus(HSPIx, HSPIS_IT_ID_RXTF))
	{
		while(!HSPIS_GetITStatus(HSPIx, HSPIS_IT_ID_RXE))
		{
			RecvBuff[RecvIndex++] = HSPIS_ReceiveData(HSPIx);
			if (DATA_SIZE == RecvIndex)
			{
				RecvIndex = 0;
				RecvFlag = 1;
			}
		}
	}
	
	if (HSPIS_GetITStatus(HSPIx, HSPIS_IT_ID_TXTF))
	{	
		HSPIS_SendData(HSPIx, SendBuff[SendIndex++]);
		if (DATA_SIZE == SendIndex)
		{
			SendIndex = 0;
		}
	}
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef	NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = HSPI0_IRQn ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}


void HSPIS_Configuration(void)
{
	HSPIS_InitTypeDef	HSPIS_InitStruct;
	
	GPIO_PinRemapConfig(GPIOC,  GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, GPIO_Remap_0);

	HSPIS_DeInit(HSPIx);
	
	HSPIS_InitStruct.HSPIS_Sample = HSPIS_SAMPLE_MODE_0;	
	HSPIS_InitStruct.HSPIS_RXFIFOFullThreshold = HSPIS_TX_FIFO_FULL_THRESHOLD_1;
	HSPIS_InitStruct.HSPIS_TXFIFOEmptyThreshold	= HSPIS_TX_FIFO_EMPTY_THRESHOLD_8;
	HSPIS_InitStruct.HSPIS_TXEnable = ENABLE;
	HSPIS_InitStruct.HSPIS_RXEnable	= ENABLE;
	
	HSPIS_FIFOReset(HSPIx, HSPIS_FIFO_TX | HSPIS_FIFO_RX);
	HSPIS_Init(HSPIx, &HSPIS_InitStruct);
	
	HSPIS_ITConfig(HSPIx, HSPIS_IT_EN, ENABLE);
	HSPIS_ITConfig(HSPIx, HSPIS_IT_RX, ENABLE);
	HSPIS_ITConfig(HSPIx, HSPIS_IT_TX, ENABLE);
}


void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStruct;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	UART_InitStruct.UART_BaudRate = 115200;
	UART_InitStruct.UART_WordLength = UART_WordLength_8b;
	UART_InitStruct.UART_StopBits = UART_StopBits_1;
	UART_InitStruct.UART_Parity = UART_Parity_No;

	UART_Init(UARTx, &UART_InitStruct);
}


void DataPrint(uint8_t *Buff, uint32_t Len)
{
	while (Len)
	{
		if (Len-- % 16)
		{
			printf("%02X ", *Buff++);
		}
		else
		{
			printf("\n%02X ", *Buff++);			
		}
	}
	
	printf("\n");
}


int DataCheck(uint8_t *Src, uint8_t *Dst, uint32_t Len)
{
	if (memcmp(Src, Dst, Len))
	{
		printf(">> Fail !\n");
		return 1;
	}
	else
	{
		printf(">> OK !\n");
		return 0;
	}
}


int fputc(int ch, FILE *f)
{
  while(!UART_IsTXEmpty(UARTx));
  UART_SendData(UARTx, (uint8_t) ch);
  return ch;
}


#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1);
}

#endif
