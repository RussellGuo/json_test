#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "systick.h"

#define	BUZZER_IO_GROUP		      GPIOC
#define	BUZZER_IO_PIN			  GPIO_Pin_9
#define	BUZZER_IO_TYPE_PWM	      GPIO_Remap_2
#define	BUZZER_IO_TYPE_IO		  GPIO_Remap_1
#define	BUZZER_PWM_TIMER		  TIMM0
#define BUZZER_PWM_TIM            TIM_7

void UART_Configuration(void);
void BUZZER_Open(int beep_freq);
void BUZZER_Close(void);

int main(int argc , char *argv[])
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
	SYSTICK_Init();
    UART_Configuration();
	
	printf("MegaHunt SCPU Timer Buzzer Demo V1.0.\n");
	printf(">>>  beep freq: 1000Hz\n");
	BUZZER_Open(1000);
	mdelay(2000);
	BUZZER_Close();	
	mdelay(5000);
	
	printf(">>>  beep freq: 3000Hz\n");	
	BUZZER_Open(3000);
	mdelay(2000);
	BUZZER_Close();
	
	while(1)
	{

	}
}

void BUZZER_PinConfig(GPIO_RemapTypeDef remap)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = BUZZER_IO_PIN;
	GPIO_InitStruct.GPIO_Remap = remap;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BUZZER_IO_GROUP, &GPIO_InitStruct);
	
	GPIO_ResetBits(BUZZER_IO_GROUP, BUZZER_IO_PIN);
}

void BUZZER_Open(int beep_freq)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;
	TIM_PWMInitTypeDef TIMER_PWMSetStruct;
	SYSCTRL_ClocksTypeDef clocks;
	
	GPIO_PinRemapConfig(BUZZER_IO_GROUP, BUZZER_IO_PIN, BUZZER_IO_TYPE_PWM);
	
	SYSCTRL_GetClocksFreq(&clocks);
	Period = clocks.PCLK_Frequency / beep_freq;
	DutyCyclePeriod = Period / 2;

	TIMER_PWMSetStruct.TIMx                = BUZZER_PWM_TIM;
	TIMER_PWMSetStruct.TIM_LowLevelPeriod  = DutyCyclePeriod;
	TIMER_PWMSetStruct.TIM_HighLevelPeriod = DutyCyclePeriod;
	TIM_PWMInit(BUZZER_PWM_TIMER, &TIMER_PWMSetStruct);

	TIM_SetPWMPeriod(BUZZER_PWM_TIMER, BUZZER_PWM_TIM, TIMER_PWMSetStruct.TIM_LowLevelPeriod, TIMER_PWMSetStruct.TIM_HighLevelPeriod);
	
	TIM_Cmd(BUZZER_PWM_TIMER, BUZZER_PWM_TIM, ENABLE);
}

void BUZZER_Close(void)
{  
	TIM_Cmd(BUZZER_PWM_TIMER, BUZZER_PWM_TIM, DISABLE);   
    
	BUZZER_PinConfig(BUZZER_IO_TYPE_IO);
	GPIO_SetBits(BUZZER_IO_GROUP, BUZZER_IO_PIN);
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
        printf("%s\t%s\t%d\r\n", __func__, file, line);
	}
}
#endif
