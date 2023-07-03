
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_gpio.h"

#define DATA_SIZE 									256														
#define HSPISx 										HSPIS0	
#define UARTx										UART0
																
void HSPIS_Configuration(void);
void UART_Configuration(void);
void DataPrint(uint8_t *Buff, uint32_t Len);
int DataCheck(uint8_t *Src, uint8_t *Dst, uint32_t Len);
																
uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];

int main (void)
{
	uint16_t i;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI1, ENABLE);
	
	UART_Configuration();
	HSPIS_Configuration();
	
	printf("MH2101 HSPIS Polling Tx Rx Demo V1.0\n");

	for(i = 0; i < DATA_SIZE; SendBuff[i] = i, i++);
	
	for(i = 0; i < DATA_SIZE; i++)
	{
		while(HSPIS_GetFlagStatus(HSPISx, HSPIS_FLAG_RXE));
		RecvBuff[i] = HSPIS_ReceiveData(HSPISx);			
		HSPIS_SendData(HSPISx, SendBuff[i]);
	}	
	
	if (DataCheck(SendBuff, RecvBuff, DATA_SIZE))
	{
		DataPrint(RecvBuff, DATA_SIZE);
	}
	
	while(1);
}


void HSPIS_Configuration(void)
{
	HSPIS_InitTypeDef	HSPIS_Initstruct;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, GPIO_Remap_0);

	HSPIM_Cmd(HSPIM0, DISABLE);	
	HSPIM_Cmd(HSPIM1, DISABLE);	
	
	HSPIS_Initstruct.HSPIS_Sample = HSPIS_SAMPLE_MODE_0;	
	HSPIS_Initstruct.HSPIS_RXFIFOFullThreshold = HSPIS_RX_FIFO_FULL_THRESHOLD_32;
	HSPIS_Initstruct.HSPIS_TXFIFOEmptyThreshold	= HSPIS_TX_FIFO_EMPTY_THRESHOLD_31;
	HSPIS_Initstruct.HSPIS_TXEnable = ENABLE;
	HSPIS_Initstruct.HSPIS_RXEnable = ENABLE;
	
	HSPIS_FIFOReset(HSPISx, HSPIS_FIFO_TX | HSPIS_FIFO_RX);
	HSPIS_Init(HSPISx, &HSPIS_Initstruct);
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

