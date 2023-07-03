#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef enum
{
	TAMPER_STATIC_ALL = 0,
	TAMPER_STATIC_4_DYNAMIC_1,
	TAMPER_DYNAMIC_ALL
}EXT_TAMPER_CONFIG_T;


void NVIC_Configuration(void);
void UART_Configuration(void);
void SENSOR_Configuration(void);

void clearBPK(void);
void setBPK(void);
void printBPK(void);

int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

	UART_Configuration();
	printf("MegaHunt SCPU SENSOR Demo V1.0.\n");
	
	SENSOR_Configuration();
	NVIC_Configuration();
    
	while(1)
	{

	}
}

void SENSOR_Configuration(void)
{
	int TAMPER_Config;
	SENSOR_EXTInitTypeDef SENSOR_EXTInitStruct;
	memset(&SENSOR_EXTInitStruct, 0, sizeof(SENSOR_EXTInitStruct));

	TAMPER_Config = TAMPER_STATIC_ALL;
	
	switch (TAMPER_Config)
	{
		case TAMPER_STATIC_4_DYNAMIC_1:
			SENSOR_EXTInitStruct.SENSOR_Port_Static = SENSOR_Port_S0 | SENSOR_Port_S1 | SENSOR_Port_S2 | SENSOR_Port_S3;
			SENSOR_EXTInitStruct.SENSOR_Port_Dynamic = SENSOR_Port_S45;	
			SENSOR_EXTInitStruct.SENSOR_Port_Pull = SENSOR_Port_S0 | SENSOR_Port_S1 | SENSOR_Port_S2 | SENSOR_Port_S3 | SENSOR_Port_S6 | SENSOR_Port_S7;
			SENSOR_EXTInitStruct.SENSOR_DynamicFrequency = SENSOR_DynamicFrequency_31_25ms;
			SENSOR_EXTInitStruct.SENSOR_GlitchEnable = ENABLE;
			SENSOR_EXTInitStruct.SENSOR_Dynamic_Sample = SENSOR_DYNAMIC_SAMPLE_2;		
			break;

		case TAMPER_DYNAMIC_ALL:
			SENSOR_EXTInitStruct.SENSOR_Port_Dynamic = SENSOR_Port_S01 | SENSOR_Port_S23 | SENSOR_Port_S45;
			SENSOR_EXTInitStruct.SENSOR_Port_Pull = SENSOR_Port_S6 | SENSOR_Port_S7;        
			SENSOR_EXTInitStruct.SENSOR_DynamicFrequency = SENSOR_DynamicFrequency_31_25ms;
			SENSOR_EXTInitStruct.SENSOR_GlitchEnable = ENABLE;
			SENSOR_EXTInitStruct.SENSOR_Dynamic_Sample = SENSOR_DYNAMIC_SAMPLE_2;					
			break;
		
		case TAMPER_STATIC_ALL:
		default:
			SENSOR_EXTInitStruct.SENSOR_Port_Static = SENSOR_Port_S0 | SENSOR_Port_S1 | SENSOR_Port_S2 | SENSOR_Port_S3 |
													  SENSOR_Port_S4 | SENSOR_Port_S5;

            /* 注意：作为输入的Tamper管脚不能悬空 */
			SENSOR_EXTInitStruct.SENSOR_Port_Pull = SENSOR_Port_S0 | SENSOR_Port_S1 | SENSOR_Port_S2 | SENSOR_Port_S3 |
													SENSOR_Port_S4 | SENSOR_Port_S5 | SENSOR_Port_S6 | SENSOR_Port_S7;
		
			SENSOR_EXTInitStruct.SENSOR_Static_Sample = SENSOR_STATIC_SAMPLE_2;
			SENSOR_EXTInitStruct.SENSOR_GlitchEnable = ENABLE;
		break;		
	}
	
	SENSOR_EXTInitStruct.SENSOR_Port_Enable = ENABLE;		

    SENSOR_ClearITPendingBit();
	SENSOR_EXTCmd(DISABLE);
	while(SET == SENSOR_EXTIsRuning());
    
	SENSOR_EXTInit(&SENSOR_EXTInitStruct);
  	SENSOR_AttackRespMode(SENSOR_Interrupt);
	SENSOR_EXTCmd(ENABLE);

	SENSOR_ANACmd(SENSOR_ANA_VOL_HIGH | SENSOR_ANA_VOL_LOW|
				  SENSOR_ANA_TEMPER_HIGH | SENSOR_ANA_TEMPER_LOW|
				  SENSOR_ANA_XTAL32K | SENSOR_ANA_MESH | SENSOR_ANA_VOLGLITCH, DISABLE);
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

	printBPK();
	setBPK();
	printBPK();
	
	NVIC_ClearPendingIRQ(SENSOR_IRQn);
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
