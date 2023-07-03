#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

  
void NVIC_Configuration(void);
void SPI_Configuration(void);
void UART_Configuration(void);
void DMA_Configuration(void);
void DataPrintf(uint8_t *buf, uint32_t bufsize);

#define SEND_BUF_SIZE				256
#define RECE_BUF_SIZE				256

uint8_t send_buf[SEND_BUF_SIZE] = {0x00};
uint8_t rece_buf[RECE_BUF_SIZE] = {0x00};

uint32_t send_buf_index = 0;
uint32_t rece_buf_index = 0;

SPI_TypeDef *SPIx = SPIM2;

int main(void)
{
	uint32_t i;
    
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_SPI2, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI2, ENABLE);
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	NVIC_Configuration();
    UART_Configuration();
    printf("MegaHunt SCPU SPI Master Tx And Rx Demo V1.0.\n");
	
    //Send Data Init 0x50 To 0x5F
	printf("Send Data Init \n");
	for(i = 0;i < SEND_BUF_SIZE; i++)
	{
		send_buf[i] = (uint8_t)((i & 0x0F) | 0x50);
	}
	
	printf("Send Data Buf\n");
	DataPrintf(send_buf, SEND_BUF_SIZE);
	printf("Reve Data Buf\n");
	DataPrintf(rece_buf, RECE_BUF_SIZE);
	SPI_Configuration();
    DMA_Configuration();
	
	//Clear RX FIFO
	printf("Rx FIFO Clean\n");
	while(SPI_GetFlagStatus(SPIx, SPI_FLAG_RXNE))
		SPI_ReceiveData(SPIx);
	
	printf("Master Send and Reve Start\n\n");
	
	for(i = 0;i < (SEND_BUF_SIZE / 8); i++)
	{
		while(DMA_IsChannelEnabled(DMA_Channel_0) != DISABLE);
		DMA_ChannelCmd(DMA_Channel_0, ENABLE);
	}
	printf("Master Send and Reve End\n");
	
	printf("Send Data Buf\n");
	DataPrintf(send_buf, SEND_BUF_SIZE);
	printf("Reve Data Buf\n");
	DataPrintf(rece_buf, RECE_BUF_SIZE);
	
	while(1);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	 
    NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_DMAInitTypeDef SPI_DMAInitStructure;
 
    //SPI2
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_0);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_0;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_8;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_7;  
	
	SPI_DMAInitStructure.SPI_DMAReq = SPI_DMAReq_Tx | SPI_DMAReq_Rx;
	SPI_DMAInitStructure.SPI_DMATransmitLevel = SPI_DMATransmitLevel_0;
	SPI_DMAInitStructure.SPI_DMAReceiveLevel = SPI_DMAReceiveLevel_1;
	SPI_DMAInitStructure.SPI_DMAEnCmd = ENABLE;

	SPI_Init(SPIx, &SPI_InitStructure);
	SPI_DMAInit(SPIx, &SPI_DMAInitStructure);
	SPI_Cmd(SPIx, ENABLE);
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;

	//memory to SPI0
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(SPIx);
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&send_buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_BlockSize = 8;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
    DMA_Init(DMA_Channel_0,&DMA_InitStruct);
    
	//SPI0 to memory
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(SPIx);
	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&rece_buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = 1;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	DMA_Init(DMA_Channel_1,&DMA_InitStruct);
    DMA_ITConfig(DMA_Channel_1,DMA_IT_BlockTransferComplete,ENABLE);
	DMA_ChannelCmd(DMA_Channel_1, ENABLE);
	DMA_Cmd(ENABLE);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0|GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
    
    UART_Init(UART0, &UART_InitStructure);
}

void DataPrintf(uint8_t *buf, uint32_t bufsize)
{
	uint32_t i = 0;
	if(0 != bufsize)
	{
		for(i = 0; i < bufsize; i++)
		{
			if(0 != i && 0 == i%16)
				printf("\n");
			printf("%02X",buf[i]);
		}
	}
	printf("\n\n");
}

//Retarget Printf
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
