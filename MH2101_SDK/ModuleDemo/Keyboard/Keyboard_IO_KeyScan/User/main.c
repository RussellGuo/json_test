#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "keyboard.h"

void UART_Configuration(void);
void SysTick_Configuration(void);
void Keyboard_Show(uint8_t keyVal);

int main(void)
{  
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();   
    
	Keyboard_Init();
    Keyboard_CallbackRegister(Keyboard_Show);
    SysTick_Configuration();

	printf("MegaHunt SCPU KeyBoard Demo V1.0.\n");

	while (1)
	{

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

void Keyboard_Show(uint8_t keyVal)
{
    printf("KEY[%d] PRESS\n", keyVal);
}

void SysTick_Configuration(void)
{
    SYSCTRL_ClocksTypeDef clock;
    
    SYSCTRL_GetClocksFreq(&clock);
	SysTick_Config(clock.CPU_Frequency / 100);
}

/* Retarget Printf */
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

