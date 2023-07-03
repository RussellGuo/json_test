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


void EXTI0_IRQHandler(void)
{
	printf("EXTI0_IRQHandler In\n");
	printf("EXTI0_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line0));
	EXTI_ClearITPendingBit(EXTI_Line0);
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler(void)
{
	printf("EXTI1_IRQHandler In\n");
	printf("EXTI1_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line1));
	EXTI_ClearITPendingBit(EXTI_Line1);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

void EXTI2_IRQHandler(void)
{
	printf("EXTI2_IRQHandler In\n");
	printf("EXTI2_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line2));
	EXTI_ClearITPendingBit(EXTI_Line2);
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void)
{
	printf("EXTI3_IRQHandler In\n");
	printf("EXTI3_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line3));
	EXTI_ClearITPendingBit(EXTI_Line3);
	NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

void EXTI4_IRQHandler(void)
{
	printf("EXTI4_IRQHandler In\n");
	printf("EXTI4_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line4));
	EXTI_ClearITPendingBit(EXTI_Line4);
	NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

void EXTI5_IRQHandler(void)
{
	printf("EXTI5_IRQHandler In\n");
	printf("EXTI5_GPIO_Status %08X\n", EXTI_GetITLineStatus(EXTI_Line5));
	EXTI_ClearITPendingBit(EXTI_Line5);
	NVIC_ClearPendingIRQ(EXTI5_IRQn);
}
