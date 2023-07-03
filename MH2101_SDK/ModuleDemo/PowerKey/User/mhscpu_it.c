#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	printf("SysTick_Handler\r\n");
}

extern volatile uint8_t key_pressed;
extern volatile uint32_t timer_count;


void EXTI5_IRQHandler(void)
{
	printf("EXTI5_IRQHandler In\n");
    
    if (Bit_RESET == GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7))   /* POWER KEY PRESSED */
    {
        key_pressed = 1;
        timer_count = 0;
        TIM_Cmd(TIMM0, TIM_1, ENABLE);  
    }
    else      /* POWER KEY RELEASE */
    {
        key_pressed = 0;
        timer_count = 0;
        TIM_Cmd(TIMM0, TIM_1, DISABLE);
    }
    
	EXTI_ClearITPendingBit(EXTI_Line5);
	NVIC_ClearPendingIRQ(EXTI5_IRQn);
    
   printf("EXTI5_IRQHandler Out, key_pressed = %d\n", key_pressed);
}

void TIM0_1_IRQHandler(void)
{ 
    timer_count++;
    
	TIM_ClearITPendingBit(TIMM0, TIM_1);
    NVIC_ClearPendingIRQ(TIM0_1_IRQn);
}
