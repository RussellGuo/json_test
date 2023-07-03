#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_gpio.h"

#define DATA_SIZE									256	
#define BLOCK_SIZE									1												
#define HSPIx										HSPIS0
#define UARTx										UART0																
																
void HSPIS_Configuration(uint8_t SampleMode);
void UART_Configuration(void);
void DMA_Configuration(void);
void DataPrint(uint8_t *Buff, uint32_t Len);
int DataCheck(uint8_t *Src, uint8_t *Dst, uint32_t Len);

uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];

DMA_TypeDef* DMA_ChannelTx = DMA_Channel_0;
DMA_TypeDef* DMA_ChannelRx = DMA_Channel_1;

int main(void)
{
	uint32_t i;
	
	
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	
	UART_Configuration();
	
	printf("MH2101 HSPI Slave DMA Tx Rx Demo v1.0\n");
	
	for(i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i & 0xFF;
		RecvBuff[i] = 0;
	}
		
	HSPIS_Configuration(HSPIS_SAMPLE_MODE_0);
	
	DMA_Configuration();
	
	for(i = 0; i < DATA_SIZE/BLOCK_SIZE; i++)
	{
		DMA_ChannelCmd(DMA_ChannelRx, ENABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelRx) != DISABLE);		
		DMA_ChannelCmd(DMA_ChannelTx, ENABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelTx) != DISABLE);		
	} 
	
	if (DataCheck(SendBuff, RecvBuff, DATA_SIZE))
	{
		DataPrint(RecvBuff, DATA_SIZE);
	}
	
	while(1);
}


void HSPIS_Configuration(uint8_t SampleMode)
{
	HSPIS_InitTypeDef		HSPIS_InitStruct;
	HSPIS_DMAInitTypeDef	HSPIS_DMAInitStruct;
	

	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, GPIO_Remap_0);
	
	HSPIM_Cmd(HSPIM0, DISABLE);	
	HSPIM_Cmd(HSPIM1, DISABLE);	
	HSPIS_DeInit(HSPIx);
	
	HSPIS_DMAInitStruct.HSPIS_DMATransmitEnable = ENABLE;
	HSPIS_DMAInitStruct.HSPIS_DMATransmitLevel = HSPIS_DMA_TRANSMIT_LEVEL_1;
	HSPIS_DMAInitStruct.HSPIS_DMAReceiveEnable = ENABLE;
	HSPIS_DMAInitStruct.HSPIS_DMAReceiveLevel = HSPIS_DMA_RECEIVE_LEVEL_1;
	HSPIS_DMAInit(HSPIx, &HSPIS_DMAInitStruct);	

	HSPIS_InitStruct.HSPIS_Sample = SampleMode;	
	HSPIS_InitStruct.HSPIS_RXFIFOFullThreshold = HSPIS_RX_FIFO_FULL_THRESHOLD_32;
	HSPIS_InitStruct.HSPIS_TXFIFOEmptyThreshold = HSPIS_TX_FIFO_EMPTY_THRESHOLD_0;
	HSPIS_InitStruct.HSPIS_TXEnable = ENABLE;
	HSPIS_InitStruct.HSPIS_RXEnable = ENABLE;
	
	HSPIS_FIFOReset(HSPIx, HSPIS_FIFO_TX | HSPIS_FIFO_RX);
	HSPIS_Init(HSPIx, &HSPIS_InitStruct);
}


void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(HSPIx);
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&HSPIx->WDR;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SendBuff;
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;	
	DMA_Init(DMA_ChannelTx, &DMA_InitStruct);
	
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(HSPIx);
	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&HSPIx->RDR;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)RecvBuff;
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;	
	DMA_Init(DMA_ChannelRx, &DMA_InitStruct);
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
