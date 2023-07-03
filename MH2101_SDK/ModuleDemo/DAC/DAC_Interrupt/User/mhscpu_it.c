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
	
}


extern uint16_t sinTable[256];
uint32_t DAC_IRQindex = 0;

/**
  * @brief  This function handles DAC interrupt request.
  * @param  None
  * @retval None
  */
void DAC_IRQHandler(void)
{
    if (SET == DAC_GetITStatus(DAC_IT_FIFO_THR))
    {
        while (SET == DAC_GetFlagStatus(DAC_FLAG_RUNNING));
		
        DAC_SetData(sinTable[DAC_IRQindex++]);
        DAC_IRQindex = DAC_IRQindex % 256;
    }
	
    if(SET == DAC_GetITStatus(DAC_IT_FIFO_OVERFLOW))
    {
        printf("dac fifo overflow irq in.\n");
        DAC_ClearITPendingBit(DAC_IT_FIFO_OVERFLOW);		
    }
    
	NVIC_ClearPendingIRQ(DAC_IRQn);
}
