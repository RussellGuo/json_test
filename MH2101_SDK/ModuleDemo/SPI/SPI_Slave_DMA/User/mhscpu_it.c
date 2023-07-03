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
extern uint8_t flag;
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
    if (CoreDebug->DHCSR & 1){  // check C_DEBUGEN == 1 -> Debugger Connected
        __breakpoint(0);    // halt program execution here
    }
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

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void DMA0_IRQHandler(void)
{
	//DMA_Channel_1 SPI Receve
    if (DMA_GetITStatus(DMA_Channel_1,DMA_IT_BlockTransferComplete) == SET)
    {
        DMA_ClearITPendingBit(DMA_Channel_1,DMA_IT_BlockTransferComplete);
//        DMA_ChannelCmd(DMA_Channel_1, ENABLE);
        flag = 1;
    }
	//DMA_Channel_0 SPI Send
//    if (DMA_GetITStatus(DMA_Channel_0,DMA_IT_BlockTransferComplete) == SET)
//    {
//        DMA_ClearITPendingBit(DMA_Channel_0,DMA_IT_BlockTransferComplete);
// 		  DMA_SetSRCAddress(DMA_Channel_0, (uint32_t)&send_buf[0]);
//        DMA_ChannelCmd(DMA_Channel_0, ENABLE);
//    }
    NVIC_ClearPendingIRQ(DMA_IRQn);
}

/**/
