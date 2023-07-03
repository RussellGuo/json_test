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

  
typedef enum MENU_TYPE_E
{
	MENU_TYPE_GROUP_SEL = 0,
	MENU_TYPE_MODE_SEL,
	MENU_TYPE_PIN_SEL,
	NB_OF_MENU_TYPE
}MENU_TYPE_T;	
  

void UART_Configuration(void);
void GPIO_Configuration(void);
void PrintMenu(MENU_TYPE_T type);
GPIO_TypeDef *GetIOGroup(uint8_t index);
int32_t GPIO_PinTest(uint8_t pinIndex, int32_t OptFlag);

GPIO_TypeDef *GPIOx;
GPIO_InitTypeDef GPIO_InitStruct;


int main(void)
{
	uint8_t tmpUartRece = 0;
	int32_t tmpIOOptFlag = 0;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	
	printf("MegaHunt SCPU GPIO Demo V1.0.\r\n");
        
	while(1)
	{
LABLE:
		PrintMenu(MENU_TYPE_GROUP_SEL);
		while (1)
		{	
			if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
			{
				if (NULL == (GPIOx = GetIOGroup(UART_ReceiveData(UART0))))
				{
					printf("GPIO select error\n");
					continue;
				}
				else
				{
					break;
				}
			}
		}
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
		GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;

		PrintMenu(MENU_TYPE_MODE_SEL);
        while (1)
		{	
			if(UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
			{
				GPIO_InitStruct.GPIO_Mode = 0;
				switch (UART_ReceiveData(UART0))
				{
				case '1':
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                    printf("GPIO_Mode_IN_FLOATING\n");
					break;
				
				case '2':
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
                    printf("GPIO_Mode_IPU\n");
					break;
				
				case '3':
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
                    printf("GPIO_Mode_Out_OD\n");
					break;
				
				case '4':
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD_PU;
                    printf("GPIO_Mode_Out_OD_PU\n");
					break;
				
				case '5':
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
                    printf("GPIO_Mode_Out_PP\n");
					break;
				}

				if (GPIO_InitStruct.GPIO_Mode == 0)
				{
					printf("GPIO_Mode select error\n");
					continue;
				}
				else
				{
					break;
				}
			}
		}

		if (GPIOx == GPIOA)
		{
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_0 | GPIO_Pin_1);
		}

		GPIO_Init(GPIOx,&GPIO_InitStruct);

		PrintMenu(MENU_TYPE_PIN_SEL);	
		while (1)
		{	
            if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
			{    
				tmpUartRece = UART_ReceiveData(UART0);
				if ((tmpUartRece >= '0' && tmpUartRece <= '9') || (tmpUartRece >= 'A' && tmpUartRece <= 'F'))
				{
					tmpIOOptFlag = GPIO_PinTest(tmpUartRece, tmpIOOptFlag);
				}
				else
				{
					switch (tmpUartRece)
					{
					case 'S':
						tmpIOOptFlag = 1;
						printf("tmpIOOptFlag = 1\n");
						break;
					
					case 'R':
						tmpIOOptFlag = 0;
						printf("tmpIOOptFlag = 0\n");
						break;
					
					case 'I':
						printf("Input:%#x\n", GPIO_ReadInputData(GPIOx));
						break;
					
					case 'O':
						goto LABLE;		
					}
				}
			}
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

void PrintMenu(MENU_TYPE_T type)
{
	switch (type)
	{
		case MENU_TYPE_GROUP_SEL:
			printf(">>>>>>>>>>>>Select Test GPIO Group<<<<<<<<<<<\n");
			printf("1:GPIOA\n");
			printf("2:GPIOB\n");	
			printf("3:GPIOC\n");	
			printf("4:GPIOD\n");	
			printf("5:GPIOE\n");	
			printf("6:GPIOF\n");		
			break;
		
		case MENU_TYPE_MODE_SEL:
			printf(">>>>>>>>>>>>Select Test GPIO Mode<<<<<<<<<<<<\n");
			printf("1:  GPIO_Mode_IN_FLOATING\n");
			printf("2:  GPIO_Mode_IPU\n");
			printf("3:  GPIO_Mode_Out_OD\n");
			printf("4:  GPIO_Mode_Out_OD_PU\n");
			printf("5:  GPIO_Mode_Out_PP\n");			
			break;
		
		case MENU_TYPE_PIN_SEL:
			printf(">>>>>>>>>>>>Select Test GPIO Pin<<<<<<<<<<<<<\n");
			printf("0:  GPIO_Pin_0\n");
			printf("1:  GPIO_Pin_1\n");
			printf("2:  GPIO_Pin_2\n");
			printf("3:  GPIO_Pin_3\n");
			printf("4:  GPIO_Pin_4\n");
			printf("5:  GPIO_Pin_5\n");
			printf("6:  GPIO_Pin_6\n");
			printf("7:  GPIO_Pin_7\n");
			printf("8:  GPIO_Pin_8\n");
			printf("9:  GPIO_Pin_9\n");
			printf("A:  GPIO_Pin_10\n");
			printf("B:  GPIO_Pin_11\n");
			printf("C:  GPIO_Pin_12\n");
			printf("D:  GPIO_Pin_13\n");
			printf("E:  GPIO_Pin_14\n");
			printf("F:  GPIO_Pin_15\n");
			printf("S:  SetBit\n");
			printf("R:  ResetBit\n");
			printf("I:  Read Input\n");
			printf("O:  Return\n");	
			break;
		
		default:
			break;
	}
}

GPIO_TypeDef *GetIOGroup(uint8_t index)
{
	switch (index)
	{
	case '1':
		return GPIOA;

	case '2':
		return GPIOB;

	case '3':
		return GPIOC;

	case '4':
		return GPIOD;

	case '5':
		return GPIOE;

	case '6':
		return GPIOF;

	default:
		return NULL;
	}	
}

int32_t GPIO_PinTest(uint8_t pinIndex, int32_t OptFlag)
{
	uint16_t IOPin;
	
	if (pinIndex >= '0' && pinIndex <= '9')
	{
		IOPin = GPIO_Pin_0 << (pinIndex - '0');	
	}
	else if (pinIndex >= 'A' && pinIndex <= 'F')
	{
		IOPin = GPIO_Pin_0 << (pinIndex - 'A' + 10);	
	}
	
	if (OptFlag)
	{
		GPIO_SetBits(GPIOx, IOPin);
		printf("GPIO_SetBits\t");
		printf("GPIO %c idr = %08X\n", (uint32_t)(((uint32_t)GPIOx - GPIO_BASE)/sizeof(GPIO_TypeDef) + 'A'), GPIOx->IODR);
		return 0;
	} 
	else
	{
		GPIO_ResetBits(GPIOx, IOPin);
		printf("GPIO_ResetBits\t");
		printf("GPIO %c idr = %08X\n", (uint32_t)(((uint32_t)GPIOx - GPIO_BASE)/sizeof(GPIO_TypeDef) + 'A'), GPIOx->IODR);
		return 1;
	}
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
