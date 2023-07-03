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
  
  
void UART_Configuration(void);
void WDT_Configuration(void);

int main(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	printf("\nMegaHunt SCPU WDT Demo V1.0.\n");
	
	WDT_Configuration();
	
	while(1)
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

void WDT_Configuration(void)
{
	SYSCTRL_ClocksTypeDef clock;
	
    SYSCTRL_GetClocksFreq(&clock);
	
	WDT_SetReload(clock.CPU_Frequency);
	WDT_ModeConfig(WDT_Mode_Interrupt);
	WDT_Enable();
}

void NMI_Handler(void)
{
	if (FALSE != WDT_GetITStatus())
	{
		/*	喂狗操作和清中断均可以清除看门狗中断
			1、喂狗操作：同时完成清中断和重置看门狗计数器
			2、清中断操作：只清除看门狗中断，计数器由系统自动重载
		*/
		/*	注释掉喂狗操作可测试看门狗系统复位功能 */
		
		/* 喂狗 */
//		WDT_ReloadCounter();    //清中断相当于喂狗，故该语句可省略	
		WDT_ClearITPendingBit();				
		printf("WDT Interrupt But Not Reset\n");
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
