
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DATA_SIZE					(256)
#define UARTx  						(UART0)
#define HSPIx										HSPIM0

void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode, uint8_t BaudRate);
void NVIC_Configuration(void);
void DataPrintf(uint8_t *Buff, uint32_t BuffSize);

uint8_t SendBuff[256],RecvBuff[256];
uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];
uint16_t SendIndex = 0;
uint16_t RecvIndex = 0;
uint8_t SendFlag = 0;
uint8_t RecvFlag = 0;

int main(void)
{
	uint16_t i = 0;
	uint8_t buff[DATA_SIZE];

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1 , ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	
	UART_Configuration();
	
	for ( i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i & 0xFF;
		buff[i] = SendBuff[i];
	}
		
	memset(RecvBuff, 0xA5, 256);
	
	HSPIM_Configuration(0, 16);	
	NVIC_Configuration();
	
	printf("MH1903S HSPI DEMO V1.0\n");
	
	HSPIM_ITConfig(HSPIx, HSPIM_IT_EN, ENABLE);
	HSPIM_ITConfig(HSPIx, HSPIM_IT_RX, ENABLE);
	HSPIM_ITConfig(HSPIx, HSPIM_IT_TX, ENABLE);
	
	while(1)
	{
		if (RecvFlag)
		{
			RecvFlag = 0;
			
			if (0 == memcmp(buff, RecvBuff, DATA_SIZE))
			{
				printf(">>> Test Pass !\n");
				DataPrintf(RecvBuff, DATA_SIZE);
			}
			else 
			{
				printf(">>> Test Fail !\n");
				HSPIM_ITConfig(HSPIx, HSPIM_IT_EN, DISABLE);
				HSPIM_ITConfig(HSPIx, HSPIM_IT_RX, DISABLE);
				HSPIM_ITConfig(HSPIx, HSPIM_IT_TX, DISABLE);
				DataPrintf(RecvBuff, DATA_SIZE);
			}
		}
	}
}

void HSPIM_Handler(void)
{
	HSPIM_ClearITPendingBit(HSPIx);
	
	if (HSPIM_GetITStatus(HSPIx, HSPIM_IT_ID_TXTF))
	{	
		HSPIM_SendData(HSPIx, SendBuff[SendIndex++]);
		if(DATA_SIZE == SendIndex)
		{
			SendFlag = 1;
			SendIndex = 0;
		}
	}
	
	if (HSPIM_GetITStatus(HSPIx, HSPIM_IT_ID_RXTF))
	{
		while(!HSPIM_GetFlagStatus(HSPIx, HSPIM_FLAG_RXE))
		{
			RecvBuff[RecvIndex++] = HSPIM_ReceiveData(HSPIx);
			if (DATA_SIZE == RecvIndex)
			{
				RecvFlag = 1;
				RecvIndex = 0;
			}
		}
	}
}


void HSPI1_IRQHandler(void)
{	
	HSPIM_Handler();
}


void HSPI0_IRQHandler(void)
{	
	HSPIM_Handler();
}


void HSPIM_Configuration(uint8_t SampleMode, uint8_t BaudRate)
{
	HSPIM_InitTypeDef	hspiminitstrcut;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	

	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, GPIO_Remap_0);
	
	HSPIM_DeInit(HSPIx);
	
	hspiminitstrcut.HSPIM_CPOL								= (SampleMode >> 1) & 0x01;
	hspiminitstrcut.HSPIM_CPHA								= SampleMode & 0x01;
	hspiminitstrcut.HSPIM_FirstBit							= HSPIM_FIRST_BIT_MSB;
	hspiminitstrcut.HSPIM_DivideEnable						= ENABLE;
	hspiminitstrcut.HSPIM_BaudRatePrescaler					= BaudRate;
	hspiminitstrcut.HSPIM_RXFIFOFullThreshold				= HSPIM_RX_FIFO_FULL_THRESHOLD_1;
	hspiminitstrcut.HSPIM_TXFIFOEmptyThreshold				= HSPIM_TX_FIFO_EMPTY_THRESHOLD_8;
	
	HSPIM_Init(HSPIx, &hspiminitstrcut);
	HSPIM_Cmd(HSPIx, ENABLE);
	HSPIM_FIFOReset(HSPIx, HSPIM_FIFO_TX | HSPIM_FIFO_RX);
	HSPIM_TransmitCmd(HSPIx, ENABLE);
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef nvicinitstruct;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	nvicinitstruct.NVIC_IRQChannel 							= HSPI0_IRQn;
	nvicinitstruct.NVIC_IRQChannelCmd 						= ENABLE;
	nvicinitstruct.NVIC_IRQChannelPreemptionPriority 		= 1;
	nvicinitstruct.NVIC_IRQChannelSubPriority 				= 1;
	NVIC_Init(&nvicinitstruct);
	
	nvicinitstruct.NVIC_IRQChannel 							= HSPI1_IRQn;
	nvicinitstruct.NVIC_IRQChannelCmd 						= ENABLE;
	nvicinitstruct.NVIC_IRQChannelPreemptionPriority 		= 1;
	nvicinitstruct.NVIC_IRQChannelSubPriority 				= 0;
	NVIC_Init(&nvicinitstruct);
}

void UART_Configuration(void)
{
	UART_InitTypeDef uart;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	uart.UART_BaudRate = 115200;
	uart.UART_WordLength = UART_WordLength_8b;
	uart.UART_StopBits = UART_StopBits_1;
	uart.UART_Parity = UART_Parity_No;

	UART_Init(UARTx, &uart);
}


void DataPrintf(uint8_t *Buff, uint32_t BuffSize)
{
	uint32_t i = 0;
	if(0 != BuffSize)
	{
		for(i = 0; i < BuffSize; i++)
		{
			if(0 != i && 0 == i%16)
				printf("\n");
			printf("%02X ",Buff[i]);
		}
	}
	printf("\n\n");
}

int SER_PutChar(int ch)
{
	while(!UART_IsTXEmpty(UARTx));
	UART_SendData(UARTx, (uint8_t) ch);

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

