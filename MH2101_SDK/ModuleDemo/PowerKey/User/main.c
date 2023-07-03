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


#define SOFT_SHUTDOWN_TIME        2500     /* 2.5s */

void NVIC_Configuration(void);
void UART_Configuration(void);
void TIMER_Configuration(void);
void PowerKey_Configuration(void);

uint8_t GetCmd(void);
void PrintMenu(void);
void EnterSleep(SleepMode_TypeDef mode);

volatile uint8_t key_pressed = 0;
volatile uint32_t timer_count = 0;


int main(void)
{
	uint8_t tmpCmd;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
	UART_Configuration();
    TIMER_Configuration();    
	PowerKey_Configuration();
	NVIC_Configuration();
	
	printf("MegaHunt SCPU PowerKey Demo V1.0.\r\n");

	PrintMenu();
	while (1)
	{
        if (key_pressed)   /* POWER KEY PRESSED */
        {   
            if (timer_count >= SOFT_SHUTDOWN_TIME)
            {
                printf("PowerOff until key release!\n");
                while (1 == key_pressed);
                
                key_pressed = 0;
                timer_count = 0;
                NVIC_ClearPendingIRQ(TIM0_1_IRQn);
                TIM_Cmd(TIMM0, TIM_1, DISABLE);
                
                SENSOR->SEN_ANA0 |= BIT(31);    /* PowerOff */
            }               
        }
            
		tmpCmd = GetCmd();
        switch(tmpCmd)
        {
        case '1':
            {
                printf("Start Normal Test\n");
                break;
            }
        case '2':
            {
                printf("\nEnter CPU Sleep, Waitting PowerKey wakeup...\n");
                EnterSleep(SleepMode_CpuOff);
                printf("> Wake up from SleepMode!\n");
                PrintMenu();                
                break;
            }
   
        case '3':
            {
                printf("\nEnter CPU Deep Sleep, Waitting PowerKey wakeup...\n");
                EnterSleep(SleepMode_DeepSleep);
                printf("> Wake up from DeepSleepMode!\n"); 
                PrintMenu();
                break;
            }
        default:
            break;
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

void PrintMenu(void)
{
    printf("=============== POWERKEY TEST MENU===============\n");
    printf("1: Normal test!\n");
    printf("2: CPU Sleep Wakeup test!\n");
    printf("3: Deep Sleep Wakeup test!\n");    
    printf("================== TEST MENU END=================\n");
}

void EnterSleep(SleepMode_TypeDef mode)
{
    if (SleepMode_DeepSleep == mode)
    {
        GPIO_WakeEvenDeInit();
        GPIO->WAKE_TYPE_EN = 0x0;
        GPIO_WakeEvenConfig(GPIO_PortSourceGPIOF, GPIO_Pin_7, ENABLE);
    } 

	SYSCTRL_EnterSleep(mode);
}


void PowerKey_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;   /* POWER KEY State Corresponds To PF7 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    
	GPIO_Init(GPIOF, &GPIO_InitStruct);
    
	EXTI_LineConfig(EXTI_Line5, EXTI_PinSource7, EXTI_Trigger_Rising_Falling);    
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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

void TIMER_Configuration(void)
{
    TIM_InitTypeDef TIM_InitStruct;
    
	SYSCTRL_ClocksTypeDef clocks;
	SYSCTRL_GetClocksFreq(&clocks);
	
    TIM_InitStruct.TIMx = TIM_1;
    TIM_InitStruct.TIM_Period = clocks.PCLK_Frequency/1000;
    
    TIM_Init(TIMM0, &TIM_InitStruct);
    TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
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
