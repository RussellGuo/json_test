#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "delay.h"


#define EEPROM_PAGE_SIZE	8
#define EEPROM_BYTE_SIZE	256
#define EEPROM_PAGE_NUM		32


void EEPROM_DataPrint(void);

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void UART_Configuration(void);
void I2C_Configuration(void);
void DMA_Configuration(void);


#define WRITE_EXTEND_BUF_SIZE			256
#define READ_EXTEND_BUF_SIZE			256


uint8_t writeBuf[256];
uint8_t readBuf[256];

uint16_t writeExtendPageAddr;
uint16_t writeExtendBuf[WRITE_EXTEND_BUF_SIZE];
uint32_t writeExtendBufIndex;
uint16_t readExtendBuf[READ_EXTEND_BUF_SIZE];
uint32_t readExtendBufIndex;

uint32_t page_addr;

int main(void)
{
	uint32_t i;
	
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0, ENABLE);
	
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);

    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	UART_Configuration();
	I2C_Configuration();
	DMA_Configuration();
	NVIC_Configuration();
	
	printf("MegaHunt SCPU I2C Demo V1.0.\r\n");
    
    SYSTICK_Init();
	
	//============================Clear EEPROM================================//
	printf("Clear EEPROM Begin!!!\n");
	
	for(i = 0; i < EEPROM_PAGE_SIZE - 1; i++)
	{
		writeExtendBuf[i] = I2C_ExtendData(0, I2C_DataCMD_Write, I2C_DataEndCondition_None);
	}
	writeExtendBuf[EEPROM_PAGE_SIZE - 1] = I2C_ExtendData(0, I2C_DataCMD_Write, I2C_DataEndCondition_Stop);

	//write eeprom by page
	page_addr = 0;
	for(page_addr = 0; page_addr < EEPROM_PAGE_NUM; page_addr++)
	{
		I2C_SendData(I2C0, page_addr * EEPROM_PAGE_SIZE, I2C_DataEndCondition_None);
		while(DMA_IsChannelEnabled(DMA_Channel_1));
		DMA_SetSRCAddress(DMA_Channel_1,(uint32_t)&writeExtendBuf[0]);
		DMA_ChannelCmd(DMA_Channel_1,ENABLE);
		mdelay(10);		//wait for EEPROM write period
	}
	
	printf("Clear EEPROM End!!!\n");
	
 	EEPROM_DataPrint();
	
	
	//============================write eeprom by page================================//
	
	printf("Write EEPROM by Byte, Page Data 0 - 7\n");
	
	for(i = 0; i < EEPROM_PAGE_SIZE - 1; i++)
	{
		writeExtendBuf[i] = I2C_ExtendData(i, I2C_DataCMD_Write, I2C_DataEndCondition_None);
	}
	writeExtendBuf[EEPROM_PAGE_SIZE - 1] = I2C_ExtendData(i, I2C_DataCMD_Write, I2C_DataEndCondition_Stop);
	
	//page write
	page_addr = 0;
	for(page_addr = 0; page_addr < EEPROM_PAGE_NUM; page_addr++)
	{
		I2C_SendData(I2C0, page_addr * EEPROM_PAGE_SIZE, I2C_DataEndCondition_None);
		while(DMA_IsChannelEnabled(DMA_Channel_1));
		DMA_SetSRCAddress(DMA_Channel_1,(uint32_t)&writeExtendBuf[0]);
		DMA_ChannelCmd(DMA_Channel_1,ENABLE);
		mdelay(10);		//wait for EEPROM write period
	}
	
 	EEPROM_DataPrint();
	
	printf("I2C DMA Test On EEPROM End\n");
 
	while(1);
}

void EEPROM_DataPrint(void)
{
	uint32_t i;
	
	//set read address 0
	I2C_WriteDataToDR(I2C0, I2C_ExtendData(0, I2C_DataCMD_Write, I2C_DataEndCondition_Stop));
	while(RESET == I2C_GetFlagStatus(I2C0, I2C_FLAG_TXE));
	
	//Init writeExtendBuf,Used for dummy write
	for(i = 0; i < WRITE_EXTEND_BUF_SIZE - 1; i++)
	{
		writeExtendBuf[i] = I2C_ExtendData(0, I2C_DataCMD_Read, I2C_DataEndCondition_None);
	}
	writeExtendBuf[WRITE_EXTEND_BUF_SIZE - 1] = I2C_ExtendData(0, I2C_DataCMD_Read, I2C_DataEndCondition_Stop);

	//DMA_Channel_1 is used to dummy page write
	//DMA_Channel_0 is used to read data
	page_addr = 0;
	
	//disable dma channle and set the memory address
	DMA_ChannelCmd(DMA_Channel_0, DISABLE);
	DMA_ChannelCmd(DMA_Channel_1, DISABLE);
	while(DMA_IsChannelEnabled(DMA_Channel_0) || DMA_IsChannelEnabled(DMA_Channel_1));
	
	DMA_SetDSRAddress(DMA_Channel_0,(uint32_t)&readExtendBuf[0]);
	DMA_SetSRCAddress(DMA_Channel_1,(uint32_t)&writeExtendBuf[0]);
	
	for(page_addr = 0; page_addr < EEPROM_PAGE_NUM; page_addr++)
	{
		//channel_0: i2c read data to memory
		DMA_ChannelCmd(DMA_Channel_0, ENABLE);
		//channel_1: i2c dummy write data to eeprom
		DMA_ChannelCmd(DMA_Channel_1, ENABLE);
		while (TRUE)
		{
			if((DMA_GetRawStatus(DMA_Channel_1 ,DMA_IT_BlockTransferComplete) == SET)&&
				(DMA_GetRawStatus(DMA_Channel_0 ,DMA_IT_BlockTransferComplete) == SET))
			{
				DMA_ClearITPendingBit(DMA_Channel_1, DMA_IT_BlockTransferComplete);
				DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_BlockTransferComplete);
				break;
			}
		}
	}
	
	//print read data
	for (i = 0;i < READ_EXTEND_BUF_SIZE;i++)
	{
		printf("%02X",readExtendBuf[i]);
		if(EEPROM_PAGE_SIZE - 1 == (i % EEPROM_PAGE_SIZE))
		{
			printf("\n");
		}
	}
	printf("\n");
}


void I2C_Configuration(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_DMAInitTypeDef I2C_DMAStruct;

	I2C_StructInit(&I2C_InitStruct);
	I2C_DMAStructInit(&I2C_DMAStruct);
	I2C_DMAStruct.I2C_DMAReceiveLevel = I2C_DMAReceiveLevel_1;
	I2C_DMAStruct.I2C_DMAReq = I2C_DMAReq_Tx | I2C_DMAReq_Rx;
	I2C_DMAStruct.I2C_DMAEnCmd = ENABLE;
	
	I2C_InitStruct.I2C_ClockSpeed = I2C_ClockSpeed_400KHz;
	I2C_InitStruct.I2C_Mode = I2C_Mode_Master;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_1;
	I2C_InitStruct.I2C_TargetAddress = 0x50;
	I2C_InitStruct.I2C_TargetAddressMode = I2C_TargetAddressMode_7bit;
	I2C_InitStruct.I2C_RXFIFOFullThreshold = I2C_RXFIFOFullThreshold_1;
	I2C_InitStruct.I2C_TXFIFOEmptyThreshold = I2C_TXFIFOEmptyThreshold_0;

	I2C_DMAInit(I2C0, &I2C_DMAStruct);
	I2C_Init(I2C0, &I2C_InitStruct);
	I2C_ITConfig(I2C0, I2C_IT_TX_ABRT, ENABLE);
	I2C_Cmd(I2C0, ENABLE);
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;

	//memory to I2C
	DMA_InitStruct.DMA_Peripheral = (uint32_t)I2C0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&I2C0->IC_DATA_CMD;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&writeExtendBuf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = 8;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	DMA_Init(DMA_Channel_1,&DMA_InitStruct);

	//I2C to memory
	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&readExtendBuf[0];
	DMA_Init(DMA_Channel_0, &DMA_InitStruct);
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_ClearPendingIRQ(TRNG_IRQn);
	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
	NVIC_ClearPendingIRQ(TIM0_1_IRQn);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
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
