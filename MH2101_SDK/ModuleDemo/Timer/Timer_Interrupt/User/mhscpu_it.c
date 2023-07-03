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

extern volatile uint32_t tick_Timer0[TIM_NUM];

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


void TIM0_0_IRQHandler(void)
{
	tick_Timer0[0]++;
	TIM_ClearITPendingBit(TIMM0, TIM_0);
	
	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
}

void TIM0_1_IRQHandler(void)
{
	tick_Timer0[1]++;
	TIM_ClearITPendingBit(TIMM0, TIM_1);
	
	NVIC_ClearPendingIRQ(TIM0_1_IRQn);	
}

void TIM0_2_IRQHandler(void)
{
	tick_Timer0[2]++;
	TIM_ClearITPendingBit(TIMM0, TIM_2);
	
	NVIC_ClearPendingIRQ(TIM0_2_IRQn);	
}

void TIM0_3_IRQHandler(void)
{
	tick_Timer0[3]++;
	TIM_ClearITPendingBit(TIMM0, TIM_3);
	
	NVIC_ClearPendingIRQ(TIM0_3_IRQn);	
}

void TIM0_4_IRQHandler(void)
{
	tick_Timer0[4]++;
	TIM_ClearITPendingBit(TIMM0, TIM_4);
	
	NVIC_ClearPendingIRQ(TIM0_4_IRQn);	
}

void TIM0_5_IRQHandler(void)
{
	tick_Timer0[5]++;
	TIM_ClearITPendingBit(TIMM0, TIM_5);
	
	NVIC_ClearPendingIRQ(TIM0_5_IRQn);	
}

void TIM0_6_IRQHandler(void)
{
	tick_Timer0[6]++;
	TIM_ClearITPendingBit(TIMM0, TIM_6);
	
	NVIC_ClearPendingIRQ(TIM0_6_IRQn);	
}

void TIM0_7_IRQHandler(void)
{
	tick_Timer0[7]++;
	TIM_ClearITPendingBit(TIMM0, TIM_7);
	
	NVIC_ClearPendingIRQ(TIM0_7_IRQn);	
}
