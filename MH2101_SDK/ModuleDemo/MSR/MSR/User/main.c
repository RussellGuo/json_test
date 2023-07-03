#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_msr.h"
#include "stdlib.h"
#include "string.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void UART_Configuration(void);


int main()
{
	track_data tdata[MAX_TRACK_NUM];
	int ret;
	int i, j;
	uint8_t cfmt, tflag;
	int cnts = 0;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

	UART_Configuration(); 

	printf("MH2101 MSR Test Demo V1.0\n");
	
	GPIO_FuncConfig(GPIOF, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_FUNC_ANA);
	GPIO_PullUpCmd(GPIOF,  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, DISABLE);
	msr_powerup();
	set_wakeup_status(SOFT_WAKEUP);
	init_dpu();

	set_hard_wakeup_level(0);
	sc_sleep();
	while (1)
	{
		switch (detect_swiping_card())
		{
		case DETECT_SWIPING_CARD:
			{
				printf("\r\nDetect swiping card,Times: %d \n", ++cnts);
				cfmt = tflag = 0;
				ret = get_decode_data(tdata, TRACK_SELECT_1 | TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				printf("T1 = %d, T2 = %d, T3 = %d\ttflag = %02X\n", (int)tdata[0].len, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
				if (ret == SUCCESS)		
				{
					for (i = 0; i < MAX_TRACK_NUM; i++)
					{
						if (tdata[i].len)
						{
							printf("T%d decode data:\n", (int)(i+1));
							for (j = 0; j < tdata[i].len; j++)
							{
								putchar(tdata[i].buf[j]);
							}
							printf("\n");
						}
					}
				}
			}
			break;

			
		case DETECT_HARD_WAKEUP:
			sc_sleep();			
			break;
			
		case DETECT_NO_SWIPING_CARD:
		default:
			break;
		}
	}
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
int SER_PutChar(int ch)
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

