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

#define SEND_BUF_SIZE				256
#define RECE_BUF_SIZE				256
uint8_t rece_com = 0;
uint8_t send_com = 0;

extern uint8_t data_buf[SEND_BUF_SIZE];
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

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void SPI0_IRQHandler(void)
{
	if(SPI_GetITStatus(SPIS0,SPI_IT_RXF) == SET)
	{
		data_buf[rece_buf_index] = SPI_ReceiveData(SPIS0);
		rece_buf_index++;
		if(rece_buf_index >= RECE_BUF_SIZE )
        {
			rece_buf_index = 0;
            rece_com = 1;
        }
		SPI_ClearITPendingBit(SPIS0, SPI_IT_RXF);
	}

	if(SPI_GetITStatus(SPIS0,SPI_IT_TXE) == SET)
	{
		SPI_SendData(SPIS0, data_buf[send_buf_index]);
		send_buf_index++;
		if(send_buf_index >= SEND_BUF_SIZE )
        {
			send_buf_index = 0;
            send_com = 1;
        }
		SPI_ClearITPendingBit(SPIS0, SPI_IT_TXE);
	}
	NVIC_ClearPendingIRQ(SPI0_IRQn);
}
