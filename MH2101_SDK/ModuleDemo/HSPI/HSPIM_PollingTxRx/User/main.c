
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DATA_SIZE					(256)
#define UARTx  						(UART0)
#define HSPIx						HSPIM0
void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode);
void DataPrintf(uint8_t *Buff, uint32_t BuffSize);

uint8_t SendBuff[256],RecvBuff[256];

int main(void)
{
	uint16_t i = 0;

	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1, ENABLE);
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO , ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);

	UART_Configuration();	
	HSPIM_Configuration(0);
	printf("MH1903S HSPI DEMO V1.0\n");
	
	for(i = 0; i < 256; i++)
	{
		SendBuff[i] = i & 0xFF;
	}
	
	while(1)
	{
		memset(RecvBuff, 0, 256);
		
		for(i = 0; i < 256; i++)
		{
			HSPIM_SendData(HSPIx,SendBuff[i]);		
			while(HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_RXE) == SET);
			RecvBuff[i] = HSPIM_ReceiveData(HSPIx);
		}
		
		if (0 == memcmp(SendBuff, RecvBuff, 256))
		{
			printf(">>> Test Pass!\n");
		}
		else
		{
			printf(">>>>Test Fail!\n");
			DataPrintf(RecvBuff, 256);
		}
	}
}

void HSPIM_Configuration(uint8_t SampleMode)
{
	HSPIM_InitTypeDef hspim;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	
	hspim.HSPIM_Mode = HSPIM_MODE_STANDARD;
	hspim.HSPIM_CPOL = (SampleMode >> 1) & 0x01;
	hspim.HSPIM_CPHA = SampleMode & 0x01;
	hspim.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	hspim.HSPIM_DivideEnable = ENABLE;
	hspim.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_16;
	hspim.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_8;
	hspim.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_8;
	
	HSPIM_DeInit(HSPIx);
	HSPIM_Init(HSPIx, &hspim);
	HSPIM_Cmd(HSPIx, ENABLE);
	HSPIM_TransmitCmd(HSPIx, ENABLE);
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

