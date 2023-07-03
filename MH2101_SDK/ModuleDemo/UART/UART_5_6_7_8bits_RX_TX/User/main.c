#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

void UART_Configuration(void);
void UART_Set(void);
void PrintList(void);

UART_InitTypeDef UART_InitStructure;

int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);
	
	UART_Configuration();
    
 	printf("MegaHunt SCPU UART Demo V1.0.\n");
    PrintList();

	while(1)
	{
        UART_Set();
	}
}

void PrintList(void)
{
    printf("\r\n");
    printf("*default is UART_WordLength_8b*\n");
    printf("\"0\" Set UART_WordLength_5b, Send 0x5A\n");
	printf("\"1\" Set UART_WordLength_6b, Send 0x5A\n");
    printf("\"2\" Set UART_WordLength_7b, Send 0x5A\n");
    printf("\"3\" Set UART_WordLength_8b, Send 0x5A\n");
    printf("p return to the previous screen\n");
}

void UART_Set(void)
{
    uint8_t cmd = 0;
	if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
	{
		cmd = UART_ReceiveData(UART0);	
		switch(cmd)
		{
        case '0':
			{
                UART_InitStructure.UART_WordLength = UART_WordLength_5b;
                UART_Init(UART0, &UART_InitStructure);
                UART_SendData(UART0,0x5A);
			}
			break;    
        case '1':
            {   
                UART_InitStructure.UART_WordLength = UART_WordLength_6b;
                UART_Init(UART0, &UART_InitStructure);
                UART_SendData(UART0,0x5A);
            }
            break;
        case '2':
            {
                UART_InitStructure.UART_WordLength = UART_WordLength_7b;
                UART_Init(UART0, &UART_InitStructure);
                UART_SendData(UART0,0x5A);
            }
            break;
         case '3':
            {
                UART_InitStructure.UART_WordLength = UART_WordLength_8b;
                UART_Init(UART0, &UART_InitStructure);
                UART_SendData(UART0,0x5A);
            }
            break;
        case 'p':
            {
                PrintList();
            }
            break;
        }  
    }
}    

void UART_Configuration(void)
{
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 , GPIO_Remap_0);
    
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
