#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>
#include "ring_buffer.h"

extern RingBufferTypeDef ringBufWrite;
extern RingBufferTypeDef ringBufRead;

extern uint32_t page_addr;
extern uint32_t page_addr_offset;
extern uint32_t read_bytes;

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

void I2C0_IRQHandler(void)
{
	if (I2C_GetITStatus(I2C0, I2C_IT_RXF))
	{
		//Read EEPROM Data
		ringBufferPut(&ringBufRead, (uint16_t)I2C_ReadDataFromDR(I2C0));
		I2C_ClearITPendingBit(I2C0, I2C_IT_RXF);
	}

	if (I2C_GetITStatus(I2C0, I2C_IT_TXE))
	{
		if(ringBufWrite.count)
		{
			I2C_WriteDataToDR(I2C0, ringBufferGet(&ringBufWrite));
		}
		else
		{
			I2C_ITConfig(I2C0, I2C_IT_TXE, DISABLE);
		}
		I2C_ClearITPendingBit(I2C0, I2C_IT_TXE);
	}
	
	NVIC_ClearPendingIRQ(I2C0_IRQn);
}
