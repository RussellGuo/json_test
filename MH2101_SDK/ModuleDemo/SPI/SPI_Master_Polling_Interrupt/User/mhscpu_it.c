#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define DATA_BUF_SIZE				256

extern uint16_t send_buf[DATA_BUF_SIZE];
extern uint16_t rece_buf[DATA_BUF_SIZE];

extern uint32_t send_buf_index;
extern uint32_t rece_buf_index;


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


void SPI0_IRQHandler(void)
{
	if (SPI_GetITStatus(SPIM0, SPI_IT_RXF) == SET)
	{
		rece_buf[rece_buf_index++] = SPI_ReceiveData(SPIM0);
		rece_buf_index = rece_buf_index % DATA_BUF_SIZE;

		SPI_ClearITPendingBit(SPIM0, SPI_IT_RXF);
	}

	if (SPI_GetITStatus(SPIM0,SPI_IT_TXE) == SET)
	{
		SPI_SendData(SPIM0, send_buf[send_buf_index++]);
		send_buf_index = send_buf_index % DATA_BUF_SIZE;
		if (0 == send_buf_index)
		{
			SPI_ITConfig(SPIM0, SPI_IT_TXE, DISABLE);		
		}

		SPI_ClearITPendingBit(SPIM0, SPI_IT_TXE);
	}

	NVIC_ClearPendingIRQ(SPI0_IRQn);
}
