#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


void NVIC_Configuration(void);
void UART_Configuration(void);

void clearBPK(void);
void setBPK(void);
void printBPK(void);

volatile uint32_t ssc_flag = 0;
volatile uint32_t sen_flag = 0;


int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	SENSOR_EXTCmd(DISABLE);
	SENSOR_EXTPortCmd(SENSOR_Port_All, DISABLE);
	SENSOR_ANACmd(SENSOR_ANA_MESH, DISABLE);
	SENSOR_ClearITPendingBit();
	
	UART_Configuration();
	NVIC_Configuration();
	
	printf("MegaHunt SCPU SENSOR Demo V1.0.\r\n");
	
	printf("Enable All Sensor\n");
	SSC_SENSORCmd(SSC_SENSOR_XTAL12M |
				  SSC_SENSOR_VOL_LOW |
				  SSC_SENSOR_VOL_HIGH |
				  SSC_SENSOR_VOLGLITCH, ENABLE);
	
	printf("MAIN_SEN_EN value %08X\n", SSC->MAIN_SEN_EN);
	SSC_SENSORAttackRespMode(SSC_SENSOR_Interrupt);
	
	while (1)
	{
		if (ssc_flag)
		{
			printBPK();
			setBPK();
			printBPK();
			ssc_flag = 0;
		}
		if (sen_flag)
		{
			printBPK();
			setBPK();
			printBPK();
			sen_flag = 0;
		}
	}
}


void clearBPK(void)
{
	uint32_t buf[BPK_KEY_NUM];
	uint32_t index = 0;
	
	while (BPK_IsReady() == RESET);
	
	//clear buf
	memset(buf, 0, sizeof(buf));

	printf("bufValue:\n");
	for (index = 0; index < sizeof(buf)/sizeof(buf[0]); index++)
	{
		printf("%08X ", buf[index]);
		if (3 == index % 4)
		{
			printf("\n");
		}
	}
	printf("\n");
	BPK_WriteKey(buf,sizeof(buf)/sizeof(buf[0]),0);
}

void setBPK(void)
{
	uint32_t buf[BPK_KEY_NUM];
	uint32_t index = 0;
	
	while (BPK_IsReady() == RESET);
	
	//set buf 0~X
	for (index = 0; index < sizeof(buf)/sizeof(buf[0]); index++)
	{
		buf[index] = index;
	}

	printf("bufValue:\n");
	for (index = 0; index < sizeof(buf)/sizeof(buf[0]); index++)
	{
		printf("%08X ", buf[index]);
		if (3 == index % 4)
		{
			printf("\n");
		}
	}
	printf("\n");
	BPK_WriteKey(buf,sizeof(buf)/sizeof(buf[0]),0);
}

void printBPK(void)
{
	uint32_t key[BPK_KEY_NUM];
	uint32_t index = 0;
	
	while (BPK_IsReady() == RESET);

	printf("readKey:\n");
	memset(key, 0, sizeof(key));
	BPK_ReadKey(key,sizeof(key)/sizeof(key[0]),0);
	for (index = 0; index < sizeof(key)/sizeof(key[0]); index++)
	{
		printf("%08X ", key[index]);
		if (3 == index % 4)
		{
			printf("\n");
		}
	}
	printf("\n");
	memset(key, 0, sizeof(key));
}


void SENSOR_IRQHandler(void)
{
	printf("SENSOR_IRQHandler in %08X\n",SENSOR->SEN_STATE);
	SENSOR->SEN_STATE = 0;
	NVIC_ClearPendingIRQ(SENSOR_IRQn);
	sen_flag = 1;
}

void SSC_IRQHandler(void)
{
	printf("SSC_IRQHandler in %08X\n",SSC->SSC_SR);
	
	SSC_ClearITPendingBit(SSC_ITSysXTAL12M | SSC_ITSysGlitch | SSC_ITSysVolHigh | SSC_ITSysVolLow);
	NVIC_ClearPendingIRQ(SSC_IRQn);
	ssc_flag = 1;
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = SENSOR_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SSC_IRQn;
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
