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
/*            Cortex-M4 Processor Exceptions Handlers                         */
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


void KEYBOARD_IRQHandler(void)
{
	uint32_t tmpEvenReg = 0;
	
	if (KCU_GetITStatus(KCU_IT_RELEASE))
	{
		printf("kcu release interrput in\n");
		//read Even register to clear The Interrup
		tmpEvenReg = KCU_GetEvenReg();
		printf("kcu event is : %08X\r\n",tmpEvenReg);
        printf("output pin %d\n", (tmpEvenReg >> 4) & 0x0F);
        printf("input  pin %d\n", (tmpEvenReg) & 0x0F);

	}
    else if (KCU_GetITStatus(KCU_IT_PUSH))
    {
        printf("kcu push get in\n");
        //read Even register to clear The status reg
        tmpEvenReg = KCU_GetEvenReg();
        printf("kcu event is : %08X\r\n",tmpEvenReg);
        printf("output pin %d\n", (tmpEvenReg >> 4) & 0x0F);
        printf("input  pin %d\n", (tmpEvenReg) & 0x0F);
    }
	
    NVIC_ClearPendingIRQ(KBD_IRQn);	
}
