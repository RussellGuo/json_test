#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


void clearBPK(void);
void setBPK(void);
void printBPK(void);

void UART_Configuration(void);

int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	
	printf("\nMegaHunt SCPU SSC Demo V1.0.\n");
	
	SENSOR_EXTCmd(DISABLE);
	SENSOR_EXTPortCmd(SENSOR_Port_All, DISABLE);
	SENSOR_ANACmd(SENSOR_ANA_MESH, DISABLE);
	SENSOR_ClearITPendingBit();
	
	//≤‚ ‘∂¡–¥øÿ÷∆
	printf("SSC_BPKReadWrite\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0 | SSC_BPKAccessCtrBlock_1, SSC_BPKReadWrite);
	clearBPK();
	printBPK();
	setBPK();
	printBPK();
	
	//≤‚ ‘÷ª∂¡øÿ÷∆
	printf("SSC_BPKReadOnly\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0 | SSC_BPKAccessCtrBlock_1, SSC_BPKReadOnly);
	clearBPK();
	printBPK();
	
	printf("SSC_BPKReadWrite SSC_BPKAccessCtrBlock_0\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0, SSC_BPKReadWrite);
	clearBPK();
	printBPK();
	
	printf("SSC_BPKReadWrite SSC_BPKAccessCtrBlock_1\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_1, SSC_BPKReadWrite);
	clearBPK();
	printBPK();

	
	//≤‚ ‘÷ª–¥øÿ÷∆
	printf("SSC_BPKWriteOnly\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0 | SSC_BPKAccessCtrBlock_1, SSC_BPKReadWrite);
	clearBPK();
	printBPK();
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0 | SSC_BPKAccessCtrBlock_1, SSC_BPKReadOnly);
	
	printf("SSC_BPKWriteOnly SSC_BPKAccessCtrBlock_0\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0, SSC_BPKWriteOnly);
	setBPK();
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_0, SSC_BPKReadWrite);
	printBPK();
	
	printf("SSC_BPKWriteOnly SSC_BPKAccessCtrBlock_1\n");
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_1, SSC_BPKWriteOnly);
	setBPK();
	SSC_BPKAccessCtrlConfig(SSC_BPKAccessCtrBlock_1, SSC_BPKReadWrite);
	printBPK();
	
	while(1);
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


void clearBPK(void)
{
	uint32_t buf[BPK_KEY_NUM];
	uint32_t index = 0;
	
	while (BPK_IsReady() == RESET);
	
	//clear buf
	memset(buf, 0, sizeof(buf));

	printf("clearBPK bufValue:\n");
	for(index = 0;index < sizeof(buf)/sizeof(buf[0]);index++)
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

	printf("setBPK bufValue:\n");
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
