#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

void UART_Configuration(void);
void DMA_Configuration(void);

uint32_t src_Buf[128] = {0};
uint32_t dst_Buf[128] = {0};

int main(void)
{
	uint32_t index = 0;

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU DMA Demo V1.0.\r\n");

	DMA_Configuration();
	
	//single memory to memory
	memset(src_Buf, 0, sizeof(src_Buf));
	memset(dst_Buf, 0, sizeof(dst_Buf));
	for (index = 0; index < sizeof(src_Buf)/sizeof(src_Buf[0]); index++)
	{
		src_Buf[index] = index;	
	}

	DMA_ChannelCmd(DMA_Channel_0, ENABLE);
	while (RESET == DMA_GetRawStatus(DMA_Channel_0, DMA_IT_DMATransferComplete));
	if (0 == memcmp(src_Buf, dst_Buf, sizeof(src_Buf)))
	{
		printf("SCPU DMA Test Pass\n");
	}
	else
	{
		printf("SCPU DMA Test Failed\n");
	}
	while(1)
	{}
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//memory to memory
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Memory;
	DMA_InitStruct.DMA_Peripheral = NULL;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&src_Buf[0];
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Word;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&dst_Buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Word;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = sizeof(src_Buf) / 4;
	
	DMA_Init(DMA_Channel_0,&DMA_InitStruct);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);
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
