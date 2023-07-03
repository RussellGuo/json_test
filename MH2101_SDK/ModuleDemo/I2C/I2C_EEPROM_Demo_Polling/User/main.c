#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "delay.h"

#define EEPROM_PAGE_SIZE	8
#define EEPROM_BYTE_SIZE	256
#define EEPROM_PAGE_NUM		32

void EEPROM_DataPrint(uint32_t page_addr, uint32_t data_size);
void I2C_EE_WaitEepromStandbyState(void);

/* Private function prototypes -----------------------------------------------*/
void UART_Configuration(void);
void I2C_Configuration(void);

uint8_t writeBuf[256];
uint8_t readBuf[256];

uint32_t page_addr;

int main(void)
{
	uint32_t i;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0,ENABLE);
    
	UART_Configuration();
	printf("MegaHunt SCPU I2C EEPROM Demo V1.0.\r\n");
	
	I2C_Configuration();
	SYSTICK_Init();
 
	//============================Clear EEPROM================================//
	printf("Clear EEPROM Begin!!!\n");
	
	for(i = 0; i < sizeof(writeBuf); i++)
    {
		writeBuf[i] = (uint8_t)0;
    }

	//write eeprom by page
	page_addr = 0;
	for(page_addr = 0; page_addr < EEPROM_PAGE_NUM; page_addr++)
	{
		I2C_SendData(I2C0, page_addr * EEPROM_PAGE_SIZE, I2C_DataEndCondition_None);
		I2C_SendBytes(I2C0,(writeBuf + (page_addr * EEPROM_PAGE_SIZE)), EEPROM_PAGE_SIZE ,I2C_DataEndCondition_Stop);
		mdelay(10);		//wait for EEPROM write period
	}
	
	printf("Clear EEPROM End!!!\n");
	
 	EEPROM_DataPrint(0, EEPROM_BYTE_SIZE);
	

	//============================write eeprom by byte================================//
	for(i = 0; i < sizeof(writeBuf); i++)
    {
		writeBuf[i] = (uint8_t)i;
    }
	
	printf("Write EEPROM by Byte, Data 0 - 255\n");
	
	//write eeprom by byte
	page_addr = 0;
	for(i = 0; i < EEPROM_BYTE_SIZE; i++)
	{
		I2C_SendData(I2C0, page_addr + i, I2C_DataEndCondition_None);
		I2C_SendBytes(I2C0,(writeBuf + i), 1 ,I2C_DataEndCondition_Stop);	//×Ö½ÚÊý
		mdelay(10);		//wait for EEPROM write period
	}
	
 	EEPROM_DataPrint(0, EEPROM_BYTE_SIZE);
	
	//============================write eeprom by page================================//
	for(i = 0; i < sizeof(writeBuf); i++)
    {
		writeBuf[i] = (uint8_t)((i % 0x08) + 1);
    }
	
	printf("Write EEPROM by Byte, Page Data 0 - 7\n");
	
	//write eeprom by byte
	page_addr = 0;
	for(page_addr = 0; page_addr < EEPROM_PAGE_NUM; page_addr++)
	{
		I2C_SendData(I2C0, page_addr * EEPROM_PAGE_SIZE, I2C_DataEndCondition_None);
		I2C_SendBytes(I2C0,(writeBuf + (page_addr * EEPROM_PAGE_SIZE)), EEPROM_PAGE_SIZE ,I2C_DataEndCondition_Stop);
		mdelay(10);		//wait for EEPROM write period
	}
	
 	EEPROM_DataPrint(0, EEPROM_BYTE_SIZE);
 
	printf("I2C Polling Test On EEPROM End\n");
	
	while(1);
}

void EEPROM_DataPrint(uint32_t page_addr, uint32_t data_size)
{
	uint32_t i;
	uint8_t DataBuf[256];
	
 	//read eeprom
	page_addr = 0;
	I2C_SendData(I2C0, page_addr * EEPROM_PAGE_SIZE, I2C_DataEndCondition_Stop);
	I2C_ReceiveBytes(I2C0, DataBuf, data_size, I2C_DataEndCondition_Stop);
	
	for (i = 0;i < sizeof(DataBuf);i++)
	{
		printf("%02X",DataBuf[i]);
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
	
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	I2C_StructInit(&I2C_InitStruct);
	
	I2C_InitStruct.I2C_ClockSpeed = I2C_ClockSpeed_100KHz;	
	I2C_InitStruct.I2C_Mode = I2C_Mode_Master;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_1;
	I2C_InitStruct.I2C_TargetAddress = 0x50;
	I2C_InitStruct.I2C_TargetAddressMode = I2C_TargetAddressMode_7bit;
	
	I2C_Init(I2C0,&I2C_InitStruct);

	I2C_Cmd(I2C0,ENABLE);
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
