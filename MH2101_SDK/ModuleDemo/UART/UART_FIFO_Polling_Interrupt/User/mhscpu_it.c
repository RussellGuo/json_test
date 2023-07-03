#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>

extern uint8_t tmpRece[18];
extern uint32_t flagRece;
extern uint32_t tmpReceHead, tmpReceTail;

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

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */

void UART0_IRQHandler(void)
{
	UART_TypeDef * UARTx = UART0;

	switch(UARTx->OFFSET_8.IIR & 0x0f)
	{
	case UART_IT_ID_RX_RECVD:
		{
			while(UART_IsRXFIFONotEmpty(UART0))
			{
				tmpRece[tmpReceHead] = UART_ReceiveData(UART0);
				tmpReceHead++;
				if(tmpReceHead == 15)
					tmpReceHead = 0;
			}
			flagRece = 1;
		}
		break;
	case UART_IT_ID_TX_EMPTY:
		{
			while(UART_IsTXFIFONotFull(UART0))
			{
				if(tmpReceTail == tmpReceHead)
					break;
				UART_SendData(UART0, tmpRece[tmpReceTail]);
				tmpReceTail++;
				if(tmpReceTail == 15)
					tmpReceTail = 0;
			}
			flagRece = 0;
			UART_ITConfig(UARTx, UART_IT_TX_EMPTY, DISABLE);
		}
		break;
	case UART_IT_ID_MODEM_STATUS:
		{
			uint32_t msr = UARTx->MSR;			
		}
		break;
	case UART_IT_ID_LINE_STATUS:
		{
			uint32_t lsr = UARTx->LSR;			
		}	
		break;
	case UART_IT_ID_BUSY_DETECT:
		{
			uint32_t usr = UARTx->USR;			
		}	
		break;
	case UART_IT_ID_CHAR_TIMEOUT:
		break;
	default:
		break;
	}
}

/**/
