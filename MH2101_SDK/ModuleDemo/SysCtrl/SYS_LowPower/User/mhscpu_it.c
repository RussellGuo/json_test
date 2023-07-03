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


void RTC_IRQHandler(void)
{
	RTC_ResetCounter();
	printf("RTC_IRQHandler In\n");
	RTC_ITConfig(DISABLE);	
	RTC_ClearITPendingBit();
	NVIC_ClearPendingIRQ(RTC_IRQn);
}

void SENSOR_IRQHandler(void)
{
	printf("SENSOR_IRQHandler in %08X\n",SENSOR->SEN_STATE);
	SENSOR_ClearITPendingBit();
	
	NVIC_ClearPendingIRQ(SENSOR_IRQn);
}

void EXTI2_IRQHandler(void)
{
	printf("EXTI2_IRQHandler In\n");
	EXTI_ClearITPendingBit(EXTI_Line2);
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void TIM0_0_IRQHandler(void)
{
	printf("TIM0_0_IRQHandler In\n");
	TIM_Cmd(TIMM0, TIM_0, DISABLE);
	TIM_ClearITPendingBit(TIMM0, TIM_0);
	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
}

