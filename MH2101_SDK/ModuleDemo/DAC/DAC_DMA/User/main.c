#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mhscpu.h"
#include "wave_data.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint16_t sinTable[256] = {0};

#define DMA_BLOCK_SIZE      1024
volatile uint32_t dma_blockCount = 0;  

void UART_Configuration(void);
void DAC_Configuration(uint32_t sampleRate);
void DMA_Configuration(uint32_t blockSize);
void NVIC_Configuration(void);

uint8_t get_cmd(void);
void printMenu(void);


int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC, ENABLE);
	
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
    
	UART_Configuration();
	printf("MegaHunt SCPU DAC DMA Demo V1.0.\n");
    
    wave_DataHandle();
    
	DAC_Configuration(SAMPLE_RATE);
    
    DMA_Configuration(DMA_BLOCK_SIZE);
    DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);	    
    DMA_ITConfig(DMA_Channel_0, DMA_IT_DMATransferComplete, ENABLE);
    
	NVIC_Configuration();
    
	DAC_Cmd(ENABLE);    
    DAC_DMACmd(ENABLE);
    
    DMA_ChannelCmd(DMA_Channel_0, ENABLE);
    
    while(1);
}


uint8_t get_cmd(void)
{
    if (UART_LINE_STATUS_RX_RECVD & UART_GetLineStatus(UART0))
    {
        return UART_ReceiveData(UART0);
    }
    else
    {
        return 0;
    }
}

void DMA_Configuration(uint32_t blockSize)
{
	DMA_InitTypeDef DMA_InitStruct;

	//memory to DAC
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(DAC);
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DAC_DATA;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&wavData[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_BlockSize = blockSize;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
    DMA_Init(DMA_Channel_0, &DMA_InitStruct);
}

void DAC_Configuration(uint32_t sampleRate)
{
	DAC_InitTypeDef DAC_InitStruct;
	SYSCTRL_ClocksTypeDef sclock;
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_1, GPIO_Remap_2);
	SYSCTRL_GetClocksFreq(&sclock);
	DAC_InitStruct.DAC_CurrSel = DAC_CURR_SEL_2K;
	DAC_InitStruct.DAC_FIFOThr = 10;
	DAC_InitStruct.DAC_TimerExp = ((sclock.PCLK_Frequency) / sampleRate/2) - 1;
	
	DAC_Init(&DAC_InitStruct);
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	NVIC_Init(&NVIC_InitStructure);
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

void DMA0_IRQHandler(void)
{
    uint32_t remain = 0;
    
    if (DMA_GetITStatus(DMA_Channel_0, DMA_IT_DMATransferComplete))
    {
        dma_blockCount++;
        if (dma_blockCount < WAVE_DATA_SIZE / (2*DMA_BLOCK_SIZE))
        {
            DMA_SetSRCAddress(DMA_Channel_0, (uint32_t)&wavData[dma_blockCount*DMA_BLOCK_SIZE]);
            DMA_ChannelCmd(DMA_Channel_0, ENABLE);
        }
        else if ((dma_blockCount == WAVE_DATA_SIZE / (2*DMA_BLOCK_SIZE)) && (0 != (remain = WAVE_DATA_SIZE % (2*DMA_BLOCK_SIZE))))
        {			
            DMA_Configuration(remain/2);                	
            DMA_SetSRCAddress(DMA_Channel_0, (uint32_t)&wavData[dma_blockCount*DMA_BLOCK_SIZE]);
			DMA_ITConfig(DMA_Channel_0, DMA_IT_DMATransferComplete, ENABLE);				
            DMA_ChannelCmd(DMA_Channel_0, ENABLE);            
        }
        
		DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
    }
	
    NVIC_ClearPendingIRQ(DMA_IRQn);
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
