
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DATA_SIZE										(256)
#define BLOCK_SIZE										(1)

void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode);
void DMA_Configuration(void);
void DataPrintf(uint8_t* Buff, uint32_t BuffSize);

uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];

UART_TypeDef* UARTx = UART0;
DMA_TypeDef* DMA_ChannelTx = DMA_Channel_0;
DMA_TypeDef* DMA_ChannelRx = DMA_Channel_1;

int main(void)
{
	uint16_t i = 0;

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);

	UART_Configuration();
	HSPIM_Configuration(3);	// SampleMode : 0 1 2 3
	DMA_Configuration();
	
	printf(">>> MHSCPU HSPIM DMA TX RX Demo v1.0\n");
	
	memset(RecvBuff, 0, DATA_SIZE);
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i & 0xFF;
		DMA_ChannelCmd(DMA_ChannelTx, ENABLE);
		DMA_ChannelCmd(DMA_ChannelRx, ENABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelTx) != DISABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelRx) != DISABLE);
	}
	
	DMA_Configuration();
	memset(RecvBuff, 0, DATA_SIZE);
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i & 0xFF;
		DMA_ChannelCmd(DMA_ChannelTx, ENABLE);
		DMA_ChannelCmd(DMA_ChannelRx, ENABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelTx) != DISABLE);
		while(DMA_IsChannelEnabled(DMA_ChannelRx) != DISABLE);
	}
	
	if (0 == memcmp(SendBuff, RecvBuff, DATA_SIZE))
	{
		printf(">>> test pass!\n");
		DataPrintf(RecvBuff, DATA_SIZE);
	}
	else 
	{
		printf(">>> test fail!\n");
		DataPrintf(RecvBuff, DATA_SIZE);
	}

	while(1);
}


void HSPIM_Configuration(uint8_t SampleMode)
{
	HSPIM_InitTypeDef hspim;
	HSPIM_DMAInitTypeDef hspim_dma;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);

	hspim.HSPIM_Mode = HSPIM_MODE_STANDARD;
	hspim.HSPIM_CPOL = (SampleMode >> 1) & 0x01;
	hspim.HSPIM_CPHA = SampleMode & 0x01;
	hspim.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	hspim.HSPIM_DivideEnable = ENABLE;
	hspim.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_16;
	hspim.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_1;
	hspim.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_1;
	
	hspim_dma.HSPIM_DMAReceiveEnable = ENABLE;
	hspim_dma.HSPIM_DMAReceiveLevel = HSPIM_DMA_RECEIVE_LEVEL_1;
	hspim_dma.HSPIM_DMATransmitEnable = ENABLE;
	hspim_dma.HSPIM_DMATransmitLevel = HSPIM_DMA_TRANSMIT_LEVEL_1;
	HSPIM_DMAInit(HSPIM0, &hspim_dma);
	
	HSPIM_Init(HSPIM0, &hspim);
	HSPIM_Cmd(HSPIM0, ENABLE);
	HSPIM_FIFOReset(HSPIM0, HSPIM_FIFO_TX | HSPIM_FIFO_RX);
	HSPIM_TransmitCmd(HSPIM0, ENABLE);
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef dma;

	dma.DMA_Peripheral = (uint32_t)(HSPIM0);
	dma.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&HSPIM0->WDR;
	dma.DMA_PeripheralInc = DMA_Inc_Nochange;
	dma.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	dma.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	dma.DMA_MemoryBaseAddr = (uint32_t)&SendBuff[0];
	dma.DMA_MemoryInc = DMA_Inc_Increment;
	dma.DMA_MemoryDataSize = DMA_DataSize_Byte;
	dma.DMA_MemoryBurstSize = DMA_BurstSize_1;
	dma.DMA_BlockSize = BLOCK_SIZE;
	dma.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;	
	DMA_Init(DMA_ChannelTx, &dma);
	
	dma.DMA_Peripheral = (uint32_t)(HSPIM0);
	dma.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&HSPIM0->RDR;
	dma.DMA_PeripheralInc = DMA_Inc_Nochange;
	dma.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	dma.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	dma.DMA_MemoryBaseAddr = (uint32_t)&RecvBuff[0];
	dma.DMA_MemoryInc = DMA_Inc_Increment;
	dma.DMA_MemoryDataSize = DMA_DataSize_Byte;
	dma.DMA_MemoryBurstSize = DMA_BurstSize_1;
	dma.DMA_BlockSize = BLOCK_SIZE;
	dma.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;	
	DMA_Init(DMA_ChannelRx, &dma);

	DMA_Cmd(ENABLE);
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

