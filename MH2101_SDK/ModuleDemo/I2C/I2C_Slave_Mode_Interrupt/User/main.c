#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void UART_Configuration(void);
void I2C_Configuration(void);

typedef struct 
{
	uint32_t Point;
	uint8_t rom[128];
}EEPROM_TypeDef;

EEPROM_TypeDef EEPROM;


int main(void)
{    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_I2C0, ENABLE);

	UART_Configuration();
	printf("MegaHunt SCPU I2C Slave Demo V1.0.\n");
	
	I2C_Configuration();
	NVIC_Configuration();
	
	I2C_ITConfig(I2C0, I2C_IT_RD_REQ | I2C_IT_RX_DONE, ENABLE);
	
	EEPROM.Point = 0;
	memset(EEPROM.rom, 0, sizeof(EEPROM.rom));

	while (1)
	{
		if (I2C_GetRawITStatus(I2C0, I2C_IT_RXF))
		{
			EEPROM.Point = I2C_ReadDataFromDR(I2C0) & 0x00FF;

			/*
			 *	清除I2C_IT_STOP_DET标志
			 *	通过I2C_IT_STOP_DET标志的产生判定“主设备”写操作的完成
			 */
			I2C_ClearITPendingBit(I2C0, I2C_IT_STOP_DET);
			while (!I2C_GetRawITStatus(I2C0, I2C_IT_STOP_DET))
			{
				if (I2C_GetFlagStatus(I2C0, I2C_FLAG_RXNE))
				{
					EEPROM.rom[EEPROM.Point] = I2C_ReadDataFromDR(I2C0) & 0x00FF;
					if (EEPROM.Point >= sizeof(EEPROM.rom))
					{
						EEPROM.Point = 0;
					}
					else
					{
						EEPROM.Point++;
					}
				}
			}
			I2C_ClearITPendingBit(I2C0, I2C_IT_STOP_DET);
		}
	}
}

void I2C0_IRQHandler(void)
{	
	if (I2C_GetITStatus(I2C0, I2C_IT_RD_REQ))
	{
		I2C_WriteDataToDR(I2C0, EEPROM.rom[EEPROM.Point]&0xFF);
		if (EEPROM.Point >= sizeof(EEPROM.rom))
		{
			EEPROM.Point = 0;
		}
		else
		{
			EEPROM.Point++;
		}
		I2C_ClearITPendingBit(I2C0, I2C_IT_RD_REQ);
	}

	if (I2C_GetITStatus(I2C0, I2C_IT_RX_DONE))
	{
		/*
		 *	“主设备”接收完成相应NACK后产生I2C_IT_RX_DONE
		 *	可以使用printf输出终端信息时
		 *  由于printf消耗时间较长，如果相隔“主设备”读请求请求很短会影响对I2C_IT_RD_REQ的相应
		 */
 		printf("I2C_IT_RX_DONE Interrupt In\n");
		I2C_ClearITPendingBit(I2C0, I2C_IT_RX_DONE);
	}
	
	NVIC_ClearPendingIRQ(I2C0_IRQn);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);

	NVIC_InitStructure.NVIC_IRQChannel = I2C0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void I2C_Configuration(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_StructInit(&I2C_InitStruct);

	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	I2C_InitStruct.I2C_ClockSpeed = I2C_ClockSpeed_100KHz;
	I2C_InitStruct.I2C_Mode = I2C_Mode_Slave;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_1;
	I2C_InitStruct.I2C_OwnAddress = 0x50;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_TargetAddress = 0xA0;
	I2C_InitStruct.I2C_TargetAddressMode = I2C_TargetAddressMode_7bit;
	I2C_InitStruct.I2C_RXFIFOFullThreshold = I2C_RXFIFOFullThreshold_1;
	I2C_InitStruct.I2C_TXFIFOEmptyThreshold = I2C_TXFIFOEmptyThreshold_0;

	I2C_Init(I2C0, &I2C_InitStruct);
	I2C_Cmd(I2C0, ENABLE);
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
