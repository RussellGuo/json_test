#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

//#define DMA_RX        1
#define DMA_TX          1
#define DMA_BLOCK_SIZE	4
#define BUFF_SIZE       128

void UART_Configuration(void);
void DMA_Configuration(void);

uint8_t src_Buf[BUFF_SIZE] = {0};
uint8_t dst_Buf[BUFF_SIZE] = {0};

UART_TypeDef *UARTx = UART0;

int main(void)
{
    uint32_t i,index = 0;
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 , ENABLE);

	UART_Configuration();
	DMA_Configuration();
    printf("MegaHunt SCPU UART Demo V1.0.\r\n");
     DMA_ChannelCmd(DMA_Channel_3,ENABLE);
#ifdef DMA_RX 
	while(1) 
	{   
       
		DMA_ChannelCmd(DMA_Channel_3,ENABLE);
		while(RESET == DMA_GetRawStatus(DMA_Channel_3, DMA_IT_BlockTransferComplete));
		DMA_ClearITPendingBit(DMA_Channel_3, DMA_IT_BlockTransferComplete);
		DMA_SetDSRAddress(DMA_Channel_3, (uint32_t)&dst_Buf[0]);
		for(index = 0;index < DMA_BLOCK_SIZE; index++)
        {
			printf("%02X", dst_Buf[index]);
            if(index == (DMA_BLOCK_SIZE - 1))
            {
                printf("\n");
            }
        }
	}
#endif
    
#ifdef DMA_TX
    for(index = 0;index < sizeof(src_Buf)/sizeof(src_Buf[0]);index++)
		src_Buf[index] = index;
    
    for(i = 0; i < BUFF_SIZE/DMA_BLOCK_SIZE; i++)
    {
        while(DMA_IsChannelEnabled(DMA_Channel_3) != DISABLE);
        DMA_ChannelCmd(DMA_Channel_3,ENABLE);
    }
    
    while(1)
    {
    }
#endif
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	UART_FIFOInitTypeDef UART_FIFOInitStruct;
    
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_StructInit(&UART_InitStructure);
	UART_FIFOStructInit(&UART_FIFOInitStruct);

	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_FIFOInitStruct.FIFO_Enable = ENABLE;
	UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_0;
	UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = ENABLE;

	UART_Init(UART0, &UART_InitStructure);
	UART_FIFOInit(UART0, &UART_FIFOInitStruct);
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;

#ifdef DMA_RX
	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_Peripheral = (uint32_t)UART0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(UART0->OFFSET_0.RBR);
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&dst_Buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = DMA_BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
#endif

#ifdef DMA_TX   
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	DMA_InitStruct.DMA_Peripheral = (uint32_t)UARTx;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(UARTx->OFFSET_0.THR);
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_8;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&src_Buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_8;
	DMA_InitStruct.DMA_BlockSize = DMA_BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;	
#endif

	DMA_Init(DMA_Channel_3, &DMA_InitStruct);
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
