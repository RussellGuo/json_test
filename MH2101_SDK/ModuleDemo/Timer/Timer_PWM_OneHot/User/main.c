#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef struct
{
	uint32_t PWM_HZ;
	uint32_t DutyCycleHighPluse;
	uint32_t DutyCycleLowPluse;
	SYSCTRL_ClocksTypeDef clocks;
	TIM_PWMInitTypeDef TIM_PWMSetStruct;
}timer_pwm_set;


void UART_Configuration(void);
void TIMER_Configuration(void);
void TimerPWMSetStructInit(void);
void TIMER_PWMSet(void);
void PrintSet(void);

uint16_t PWM_CurrentPin = 1;

timer_pwm_set timerPWMSet;


int main(int argc , char *argv[])
{
    uint32_t delay;
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
    
    UART_Configuration();
	TimerPWMSetStructInit();
	TIMER_Configuration();
	
	printf("MegaHunt SCPU Timer PWM Demo V1.0.\r\n");
    
    TIM_PWMSinglePulseConfig(TIMM0, TIM_3, ENABLE);
    TIM_Cmd(TIMM0, TIM_3, ENABLE);
    PrintSet();
    
	while(1);
}


void PrintSet(void)
{
	printf("PWM Width %dKHZ\n", timerPWMSet.PWM_HZ / 1000);
	printf("DutyCycle HighPluse : LowPluse = %d\n", timerPWMSet.DutyCycleHighPluse);
}

void TimerPWMSetStructInit(void)
{
	timerPWMSet.PWM_HZ = 1000;
	timerPWMSet.DutyCycleHighPluse = 1;
	timerPWMSet.DutyCycleLowPluse = 1;
    
	SYSCTRL_GetClocksFreq(&timerPWMSet.clocks);
    
    if(timerPWMSet.clocks.PCLK_Frequency < (timerPWMSet.PWM_HZ * 2))
    {
        printf("You set PWM frequency is %dHz.\n",timerPWMSet.PWM_HZ);
        printf("The highest frequency of PWM is PCLK_Frequency/2,is %dHz.\n",timerPWMSet.clocks.PCLK_Frequency / 2);
        while(1);
    }
}

void TIMER_PWMSet(void)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;

	Period = timerPWMSet.clocks.PCLK_Frequency / timerPWMSet.PWM_HZ;

	DutyCyclePeriod = Period / (timerPWMSet.DutyCycleLowPluse + timerPWMSet.DutyCycleHighPluse);

	timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleLowPluse - 1);
	timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleHighPluse - 1);

    TIM_SetPWMPeriod(TIMM0, TIM_3,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
}

void TIMER_Configuration(void)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;
	
	Period = timerPWMSet.clocks.PCLK_Frequency / timerPWMSet.PWM_HZ;
	
	DutyCyclePeriod = Period / (timerPWMSet.DutyCycleLowPluse + timerPWMSet.DutyCycleHighPluse);
	
	timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleLowPluse - 1);
	timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleHighPluse - 1);

    timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_3;
    TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_3, GPIO_Remap_2);
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
