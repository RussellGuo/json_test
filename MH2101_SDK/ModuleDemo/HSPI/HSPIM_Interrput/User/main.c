
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DATA_SIZE					(256)
#define UARTx  						(UART0)
#define DUMMY_BYTE					(0x05)

void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode);
void NVIC_Configuration(void);
void HSPI_Handler(void);

HSPIM_InitTypeDef HSPIM_InitStruct;
HSPIM_TypeDef *HSPIx = HSPIM0;
int main(void)
{
	uint16_t i = 0, j = 0;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1, ENABLE);

  
	UART_Configuration();	
	
	printf("MH2101 HSPI INTERRPUT DEMO V1.0\n");
	
	for(HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold = 1; HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold < 63; 
			HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold++ )
	{
		
		printf("\n>>>>> HSPIM_RXFIFOFullThreshold : %d\n", HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold);
		
		for(j = 0; j < 4; j++)
		{
			printf(">>>>> HSPI Mode : %d\n", j);
			
			HSPIM_Configuration(j);
			NVIC_Configuration();
			
			for(i = 0; i < HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold + 1; i++)
			{
				HSPIM_SendData(HSPIx,DUMMY_BYTE);
				while(HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_RXE) == SET);
			}
		}
	}
	
	while(1);
}


void HSPI_Handler(void)
{
	HSPIx->FLSR;
	
	if (SET == HSPIM_GetITStatus(HSPIx,HSPIM_IT_ID_TXTF))
	{	
		printf(">>HSPIM_IT_ID_TXTF\n");
		HSPIM_ITConfig(HSPIx,HSPIM_IT_TX, DISABLE);
	}
	
	if (SET == HSPIM_GetITStatus(HSPIx,HSPIM_IT_ID_RXTF))
	{
		printf(">>HSPI_IT_ID_RXTF\n");
		HSPIM_ITConfig(HSPIx,HSPIM_IT_EN, DISABLE);
	}
}

void HSPI0_IRQHandler(void)
{
	HSPI_Handler();
}

void HSPI1_IRQHandler(void)
{
	HSPI_Handler();
}

void HSPIM_Configuration(uint8_t SampleMode)
{
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	
	HSPIM_InitStruct.HSPIM_Mode = HSPIM_MODE_STANDARD;
	HSPIM_InitStruct.HSPIM_CPOL = (SampleMode >> 1) & 0x01;
	HSPIM_InitStruct.HSPIM_CPHA = SampleMode & 0x01;
	HSPIM_InitStruct.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	HSPIM_InitStruct.HSPIM_DivideEnable = ENABLE;
	HSPIM_InitStruct.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_16;
	//HSPIM_InitStruct.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_5;
	HSPIM_InitStruct.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_1;
	
	HSPIM_Init(HSPIx, &HSPIM_InitStruct);
	HSPIM_FIFOReset(HSPIx, HSPIM_FIFO_TX | HSPIM_FIFO_RX);	
	HSPIM_Cmd(HSPIx, ENABLE);
	HSPIM_TransmitCmd(HSPIx, ENABLE);
	
	HSPIM_ITConfig(HSPIx,HSPIM_IT_EN, ENABLE);
	HSPIM_ITConfig(HSPIx,HSPIM_IT_RX, ENABLE);
	HSPIM_ITConfig(HSPIx,HSPIM_IT_TX, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef nvic;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	nvic.NVIC_IRQChannel = HSPI0_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;	
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = HSPI1_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
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

