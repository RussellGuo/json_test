#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint32_t EXTI_Line;  
  
void NVIC_Configuration(void);
void UART_Configuration(void);

uint8_t GetCmd(void);


void EXTI_PrintfMenu(void)
{
    printf("=========================================\n");
    printf("0: GPIOA Interrupt test!\n");
    printf("1: GPIOB Interrupt test!\n");
    printf("2: GPIOC Interrupt test!\n");
	printf("3: GPIOD Interrupt test!\n");
	printf("4: GPIOE Interrupt test!\n");
	printf("5: GPIOF Interrupt test!\n");
	printf("P:set EXTI_Trigger_Off\n");
	printf("Q:set EXTI_Trigger_Rising\n");
	printf("R:set EXTI_Trigger_Falling\n");
	printf("S:set EXTI_Trigger_Rising_Falling\n");
    printf("=========================================\n");	
}

int main(void)
{
	uint8_t tmpCmd;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	NVIC_Configuration();
	
	printf("MegaHunt SCPU GPIO Demo V1.0.\r\n");

	EXTI_PrintfMenu();
	
	EXTI_Line = EXTI_Line1;
	
	while(1)
	{
		tmpCmd = 0;
		tmpCmd = GetCmd();
		if(tmpCmd)
		{
			switch(tmpCmd)
			{
            case '0':
				{
					printf("TEST GPIOA EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line0;
					GPIO_PinRemapConfig(GPIOA, GPIO_Pin_All, GPIO_Remap_1);
					GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
                    break;
				}
			case '1':
				{
					printf("TEST GPIOB EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line1;
					GPIO_PinRemapConfig(GPIOB, GPIO_Pin_All, GPIO_Remap_1);
                    break;
				}
			case '2':
				{
					printf("TEST GPIOC EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line2;
					GPIO_PinRemapConfig(GPIOC, GPIO_Pin_All, GPIO_Remap_1);
                    break;
				}
			case '3':
				{
					printf("TEST GPIOD EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line3;
					GPIO_PinRemapConfig(GPIOD, GPIO_Pin_All, GPIO_Remap_1);
                    break;
				}
			case '4':
				{
					printf("TEST GPIOE EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line4;
					GPIO_PinRemapConfig(GPIOE, GPIO_Pin_All, GPIO_Remap_1);
                    break;
				}
			case '5':
				{
					printf("TEST GPIOF EXTI!\n");
					EXTI_DeInit();
					EXTI_Line = EXTI_Line5;
					GPIO_PinRemapConfig(GPIOF, GPIO_Pin_All, GPIO_Remap_1);
                    break;
				}				
				
			case 'P':
			case 'p':
				{
					printf("\nset EXTI_Trigger_Off\n");
					EXTI_LineConfig(EXTI_Line, EXTI_PinSourceAll, EXTI_Trigger_Off);
					break;
				}
            case 'q':
			case 'Q':
				{
					printf("\nset EXTI_Trigger_Rising\n");
					EXTI_LineConfig(EXTI_Line, EXTI_PinSourceAll, EXTI_Trigger_Rising);
					break;
				}
            case 'r':
			case 'R':
				{
					printf("\nset EXTI_Trigger_Falling\n");
					EXTI_LineConfig(EXTI_Line, EXTI_PinSourceAll, EXTI_Trigger_Falling);
					break;
				}
            case 's':
			case 'S':
				{
					printf("\nset EXTI_Trigger_Rising_Falling\n");
					EXTI_LineConfig(EXTI_Line, EXTI_PinSourceAll, EXTI_Trigger_Rising_Falling);
					break;
				}
			default:
				{
					EXTI_PrintfMenu();
				}
			}
		}
	}
}

uint8_t GetCmd(void)
{
	uint8_t tmpCmd = 0;

	if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
	{
		tmpCmd = UART_ReceiveData(UART0);		
	}

	return tmpCmd;
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

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
