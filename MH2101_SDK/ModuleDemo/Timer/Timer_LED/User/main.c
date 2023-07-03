#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "systick.h"

#define	LED_IO_GROUP		      GPIOA
#define	LED_IO_PIN			      GPIO_Pin_2
#define	LED_IO_TYPE_PWM	          GPIO_Remap_2
#define	LED_IO_TYPE_IO		      GPIO_Remap_1
#define	LED_PWM_TIMER		      TIMM0
#define LED_PWM_TIM               TIM_2


#define	LASER_IO_GROUP		      GPIOA
#define	LASER_IO_PIN			  GPIO_Pin_3
#define	LASER_IO_TYPE_PWM	      GPIO_Remap_2
#define	LASER_IO_TYPE_IO		  GPIO_Remap_1
#define	LASER_PWM_TIMER		      TIMM0
#define LASER_PWM_TIM             TIM_3


void UART_Configuration(void);
void LED_Open(int led_freq);
void LED_Close(void);

void LASER_Open(int laser_freq);
void LASER_Close(void);

int main(int argc , char *argv[])
{   
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
	SYSTICK_Init();
    UART_Configuration();
	
	printf("MegaHunt SCPU Timer LED Demo V1.0.\n");
	
	printf(">>> flicker freq: 10KHz, DutyCycle: 90%%\n");
	LED_Open(10000);
	mdelay(5000);
	LED_Close();	

	printf(">>> laser freq: 10KHz, DutyCycle: 50%%\n");	
	LASER_Open(10000);
	mdelay(5000);	
	LASER_Close();	
	
	while(1)
	{

	}
}

void LED_PinConfig(GPIO_RemapTypeDef remap)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = LED_IO_PIN;
	GPIO_InitStruct.GPIO_Remap = remap;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_IO_GROUP, &GPIO_InitStruct);
	
	GPIO_SetBits(LED_IO_GROUP, LED_IO_PIN);
}

void LED_Open(int led_freq)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;
	TIM_PWMInitTypeDef TIMER_PWMSetStruct;
	SYSCTRL_ClocksTypeDef clocks;
	SYSCTRL_GetClocksFreq(&clocks);
	
	GPIO_PinRemapConfig(LED_IO_GROUP, LED_IO_PIN, LED_IO_TYPE_PWM);
	
	Period = (clocks.PCLK_Frequency) / led_freq;
	DutyCyclePeriod = Period / 10;

	TIMER_PWMSetStruct.TIMx                = LED_PWM_TIM;
	TIMER_PWMSetStruct.TIM_LowLevelPeriod  = DutyCyclePeriod;
	TIMER_PWMSetStruct.TIM_HighLevelPeriod = 9 * DutyCyclePeriod;
	TIM_PWMInit(LED_PWM_TIMER, &TIMER_PWMSetStruct);

	TIM_SetPWMPeriod(LED_PWM_TIMER, LED_PWM_TIM, TIMER_PWMSetStruct.TIM_LowLevelPeriod, TIMER_PWMSetStruct.TIM_HighLevelPeriod);
	
	TIM_Cmd(LED_PWM_TIMER, LED_PWM_TIM, ENABLE);
}

void LED_Close(void)
{  
	TIM_Cmd(LED_PWM_TIMER, LED_PWM_TIM, DISABLE);   
    
	LED_PinConfig(LED_IO_TYPE_IO);
	GPIO_SetBits(LED_IO_GROUP, LED_IO_PIN);
}


void LASER_PinConfig(GPIO_RemapTypeDef remap)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = LASER_IO_PIN;
	GPIO_InitStruct.GPIO_Remap = remap;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LASER_IO_GROUP, &GPIO_InitStruct);
	
	GPIO_ResetBits(LASER_IO_GROUP, LASER_IO_PIN);
}

void LASER_Open(int laser_freq)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;
	TIM_PWMInitTypeDef TIMER_PWMSetStruct;
	SYSCTRL_ClocksTypeDef clocks;
	SYSCTRL_GetClocksFreq(&clocks);
	
	GPIO_PinRemapConfig(LASER_IO_GROUP, LASER_IO_PIN, LASER_IO_TYPE_PWM);
	
	Period = (clocks.PCLK_Frequency) / laser_freq;
	DutyCyclePeriod = Period / 2;

	TIMER_PWMSetStruct.TIMx                = LASER_PWM_TIM;
	TIMER_PWMSetStruct.TIM_LowLevelPeriod  = DutyCyclePeriod;
	TIMER_PWMSetStruct.TIM_HighLevelPeriod = DutyCyclePeriod;
	TIM_PWMInit(LASER_PWM_TIMER, &TIMER_PWMSetStruct);

	TIM_SetPWMPeriod(LASER_PWM_TIMER, LASER_PWM_TIM, TIMER_PWMSetStruct.TIM_LowLevelPeriod, TIMER_PWMSetStruct.TIM_HighLevelPeriod);
	
	TIM_Cmd(LASER_PWM_TIMER, LASER_PWM_TIM, ENABLE);
}

void LASER_Close(void)
{  
	TIM_Cmd(LASER_PWM_TIMER, LASER_PWM_TIM, DISABLE);   
    
	LED_PinConfig(LASER_IO_TYPE_IO);
	GPIO_ResetBits(LASER_IO_GROUP, LASER_IO_PIN);
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
