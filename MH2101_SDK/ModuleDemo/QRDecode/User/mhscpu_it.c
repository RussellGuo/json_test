


#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>
#include "mhscpu_dcmi.h"
#include "DecodeLib.h"
#include "ProjectConfig.h"
#include "mhscpu_timer.h"

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
  printf("HardFault_Handler\n");

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
	//printf("SysTick_Handler\r\n");
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */



void UART0_IRQHandler(void)
{
}
/**
 * DCMI 中断处理函数
 *
 */

void DCMI_IRQHandler(void)
{  
	if (DCMI_GetITStatus(DCMI_IT_LINE) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_LINE); 			  
	}
	
	if (DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);	  
	}

	if (DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) 
	{
        //调用回调函数
        DCMI_CallBackFrame();
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	} 
	
	if (DCMI_GetITStatus(DCMI_IT_OVF) != RESET)
	{
		DCMI_ClearITPendingBit(DCMI_IT_OVF); 
	}
    
	if (DCMI_GetITStatus(DCMI_IT_ERR) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_ERR);
	}
}

/***********************************************************************/
/**
 * DMA中断处理函数
 *
 */
void DMA0_IRQHandler(void)
{
    uint32_t tmpChannelxBit;
    //DMA回调函数
    DMA_CallBackFrame();
    //启动数据处理
    TIMM0->TIM[CAM_TASK_TIMER].ControlReg |= TIMER_CONTROL_REG_TIMER_ENABLE;//enable timer 

    tmpChannelxBit = 0x000001 << (DMA_Channel_0 - DMA_Channel_0);
	if(DMA->StatusTfr_L & tmpChannelxBit)//如果是DMA 完成中断
	{
		DMA->ClearTfr_L = tmpChannelxBit;
	}
    
	else if(DMA->StatusErr_L & tmpChannelxBit)//error 中断
	{
        
		DMA->ClearErr_L = tmpChannelxBit;
	}
	else if(DMA->StatusSrcTran_L & tmpChannelxBit)
	{
		DMA->ClearSrcTran_L = tmpChannelxBit;

	}
	else if(DMA->StatusDstTran_L & tmpChannelxBit)
	{
		DMA->ClearDstTran_L = tmpChannelxBit;

	}
	
}

/**
 *
 *
 */
void TIM0_6_IRQHandler (void)
{
	//irq stop 
	volatile uint32_t clr;
	TIMM0->TIM[CAM_TASK_TIMER].ControlReg &= ~TIMER_CONTROL_REG_TIMER_ENABLE;       // disable 
	clr = TIMM0->TIM[CAM_TASK_TIMER].EOI;
	NVIC->ICPR[((uint32_t)(CAM_TASK_TIMER_IRQ) >> 5)] = (1 << ((uint32_t)(CAM_TASK_TIMER_IRQ) & 0x1F)); /* Clear pending interrupt */
	// set image ok event
    Image8WDataProcess();
}

/**/

