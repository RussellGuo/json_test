#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mhscpu.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint16_t sinTable[256] = {0}; 
  
void GetSinTable(uint32_t max, uint16_t *table, uint32_t size);

void UART_Configuration(void);
void DAC_Configuration(void);
void NVIC_Configuration(void);

uint8_t get_cmd(void);
void printMenu(void);


int main(void)
{
	uint32_t i, freq = 1;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC, ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU DAC Demo V1.0.\n");
    
	GetSinTable(600, sinTable, sizeof(sinTable)/sizeof(sinTable[0]));
    
	DAC_Configuration();
	NVIC_Configuration();

	printMenu();
	while (1)
	{
		switch (get_cmd())
		{
			case '+':
				freq = 2 * freq;
				if (freq < 1 || freq > 0xFFFFFFFF)
				{
					freq = 1;
				}
				printf("freq is %d.\n",freq);
				DAC->DAC_TIMER = freq;				
			break;
			
			case '-':
				freq = freq / 2;
				if(freq < 1)
				{
					freq = 1;
				}
				printf("freq is %d.\n",freq);
				DAC->DAC_TIMER = freq;				
			break;
				
			case 's':
				DAC_ClearITPendingBit(DAC_IT_FIFO_OVERFLOW);
				DAC_Cmd(ENABLE);
			
				DAC_ITConfig(DAC_IT_FIFO_THR, ENABLE);
				DAC_ITConfig(DAC_IT_FIFO_OVERFLOW, ENABLE);
				break;

			default:
				break;
		} 
	}	
}

void printMenu(void)
{
	printf("=================DAC Menu================\n");
	printf("s: start DAC\n");
	printf("+: add freq\n");	
	printf("-: dec freq\n");		
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

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  generate sin table.
  * @param  max: DA max value in a period.
  *         table£ºsin table	
  *         size£ºnumber of sampling points in a period.
  * @retval None
  */
void GetSinTable(uint32_t max, uint16_t *table, uint32_t size)
{
	uint32_t i = 0;
	float radian, angle;
	
	angle = 360.000 / size;
	
	for (i = 0; i < size; i++)
	{
		radian = angle * i;
		radian = radian * 0.01744;   ///< »¡¶È=½Ç¶È*(¦Ð/180)
		
		table[i] = (max/2) * sin(radian) + (max/2);
	}
}

void DAC_Configuration(void)
{
	DAC_InitTypeDef DAC_InitStruct;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_1, GPIO_Remap_2);
	
	DAC_InitStruct.DAC_CurrSel = DAC_CURR_SEL_2K;
	DAC_InitStruct.DAC_FIFOThr = 0x8;
	DAC_InitStruct.DAC_TimerExp = 0x100;
	
	DAC_Init(&DAC_InitStruct);
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
