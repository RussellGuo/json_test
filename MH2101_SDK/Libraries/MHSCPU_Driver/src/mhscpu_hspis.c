/************************ (C) COPYRIGHT Megahuntmicro *************************************************
 * @file                : mhscpu_hspis.c
 * @author              : Megahuntmicro
 * @version             : V1.1.20201202
 * @date                : 2-Decmber-2020
 * @brief               : This file provides all the HSPIS firmware functions
 ******************************************************************************************************/
 
#include "mhscpu_hspis.h"

/** @defgroup HSPIS_Private_Defines
  * @{
  */
  
#define HSPIS_CR_CLEAR_MASK											((uint32_t)~0x180007)
#define HSPIS_CR_DMA_TRANSMIT_LEVEL_CLEAR_MASK						((uint32_t)~0x7F800)
#define HSPIS_CR_DMA_RECEIVE_LEVEL_CLEAR_MASK						((uint32_t)~0x7F8)
#define HSPIS_FCR_CLEAR_MASK										((uint32_t)~0x3F3F3F00)

#define HSPIS_CR_PARAM_SAMPLE_POS									(0x13)
#define HSPIS_CR_PARAM_DMA_TRANSMIT_LEVEL_POS 						(0x0C)
#define HSPIS_CR_PARAM_DMA_RECEIVE_LEVEL_POS						(0x04)
#define HSPIS_CR_PARAM_DMA_TRANSMIT_ENABLE_POS						(0x0B)
#define HSPIS_CR_PARAM_DMA_RECEIVE_ENABLE_POS						(0x03)
#define HSPIS_CR_PARAM_INTERRPUT_ENABLE_POS							(0x02)
#define HSPIS_CR_PARAM_TRANSMIT_ENABLE_POS							(0x01)
#define HSPIS_CR_PARAM_RECEIVE_ENABLE_POS							(0x00)

#define HSPIS_FCR_PARAM_TRANSIMIT_FIFO_FULL_THRESHOULD_POS			(0x00)
#define HSPIS_FCR_PARAM_TRANSIMIT_FIFO_EMPTY_THRESHOULD_POS			(0x08)
#define HSPIS_FCR_PARAM_RECEIVE_FIFO_FULL_THRESHOULD_POS			(0x10)
#define HSPIS_FCR_PARAM_REVEIVE_FIFO_EMPTY_THRESHOULD_POS			(0x18)
 
 /**
  * @}
  */ 
  
 /**
  * @brief  Deinitializes the HSPIS peripheral registers to their default reset values.
  * @retval None
  */
void HSPIS_DeInit(HSPIS_TypeDef *HSPIx)
{
	//SYSCTRL_AHBPeriphResetCmd(SYSCTRL_HSPI_RESET, ENABLE);
}
 
 
/**
  * @brief  Initializes the HSPIS peripheral according to the specified 
  *         parameters in the HSPIS_InitStruct.
  * @param  HSPIS_InitStruct: pointer to a HSPIS_InitTypeDef structure that
  *         contains the configuration information for the specified HSPIS peripheral.
  * @retval None
  */
  
void HSPIS_Init(HSPIS_TypeDef *HSPIx, HSPIS_InitTypeDef* HSPIS_InitStruct)
{
	HSPIM0->CR0 &= ~BIT(24);
	HSPIM1->CR0 &= ~BIT(24);
	
	HSPIx->FCR = (uint32_t)((HSPIx->FCR & HSPIS_FCR_CLEAR_MASK)
						| ((HSPIS_InitStruct->HSPIS_TXFIFOEmptyThreshold & 0x3F) << HSPIS_FCR_PARAM_TRANSIMIT_FIFO_EMPTY_THRESHOULD_POS)
						| ((HSPIS_InitStruct->HSPIS_RXFIFOFullThreshold & 0x3F) << HSPIS_FCR_PARAM_RECEIVE_FIFO_FULL_THRESHOULD_POS));
	
	HSPIx->CR = (uint32_t)((HSPIx->CR & HSPIS_CR_CLEAR_MASK)
						| ((HSPIS_InitStruct->HSPIS_TXEnable & 0x01) << HSPIS_CR_PARAM_TRANSMIT_ENABLE_POS)
						| ((HSPIS_InitStruct->HSPIS_RXEnable & 0x01) << HSPIS_CR_PARAM_RECEIVE_ENABLE_POS)
						| ((HSPIS_InitStruct->HSPIS_Sample & 0x03) << HSPIS_CR_PARAM_SAMPLE_POS));
}


/**
  * @brief  Initializes the HSPIS DMA interface according to the specified parameters.
  * @param  HSPIS_DMAInitStruct: pointer to a HSPIS_DMAInitStruct structure that contains the configuration information.
  * @retval None
  */

void HSPIS_DMAInit(HSPIS_TypeDef *HSPIx, HSPIS_DMAInitTypeDef* HSPIS_DMAInitStruct)
{
	if (ENABLE == HSPIS_DMAInitStruct->HSPIS_DMAReceiveEnable)
	{	
		HSPIx->CR = (uint32_t)((HSPIx->CR & HSPIS_CR_DMA_RECEIVE_LEVEL_CLEAR_MASK)
						| ((HSPIS_DMAInitStruct->HSPIS_DMAReceiveLevel & 0x7F) << HSPIS_CR_PARAM_DMA_RECEIVE_LEVEL_POS)
						| ((HSPIS_DMAInitStruct->HSPIS_DMAReceiveEnable & 0x01) << HSPIS_CR_PARAM_DMA_RECEIVE_ENABLE_POS));
	}
	else
	{
		HSPIx->CR &= ~(uint32_t)((~HSPIS_DMAInitStruct->HSPIS_DMAReceiveEnable & 0x01) << HSPIS_CR_PARAM_DMA_RECEIVE_ENABLE_POS);
	}
	
	if (ENABLE == HSPIS_DMAInitStruct->HSPIS_DMATransmitEnable)
	{
		HSPIx->CR = (uint32_t)((HSPIx->CR & HSPIS_CR_DMA_TRANSMIT_LEVEL_CLEAR_MASK)
						| ((HSPIS_DMAInitStruct->HSPIS_DMATransmitLevel & 0x7F) << HSPIS_CR_PARAM_DMA_TRANSMIT_LEVEL_POS)
						| ((HSPIS_DMAInitStruct->HSPIS_DMATransmitEnable & 0x01) << HSPIS_CR_PARAM_DMA_TRANSMIT_ENABLE_POS));
	}
	else 
	{
		HSPIx->CR &= ~(uint32_t)((~HSPIS_DMAInitStruct->HSPIS_DMATransmitEnable & 0x01) << HSPIS_CR_PARAM_DMA_TRANSMIT_ENABLE_POS);
	}															 					  
}


/**
  * @brief  Transmits single data through the HSPIS peripheral.
  * @param  data: the data to be transmitted
  * @retval None
  */
void HSPIS_SendData(HSPIS_TypeDef *HSPIx, uint8_t Data)
{
	HSPIx->WDR = Data;
}

/**
  * @brief  Receive single data through the HSPIS peripheral.
  * @retval The received data
  */
uint8_t HSPIS_ReceiveData(HSPIS_TypeDef *HSPIx)
{
	return (uint8_t)HSPIx->RDR;
}


/**
  *@brief  Enable or disable the specified HSPIS interrupt.
  */

void HSPIS_ITConfig(HSPIS_TypeDef *HSPIx, uint32_t HSPIS_IT, FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		HSPIx->CR |= HSPIS_IT;
	}
	else
	{
		HSPIx->CR &= ~HSPIS_IT;
	}
}

/**
  * @brief  Checks whether the specified HSPIS flag is set or not.
  * @retval The new state of HSPIS_FLAG (SET or RESET).
  */
FlagStatus HSPIS_GetFlagStatus(HSPIS_TypeDef *HSPIx, uint32_t HSPIS_Flag)
{
	if ((HSPIx->SR & HSPIS_Flag) != RESET)
	{
		return SET;
	} 
	return RESET;
}

/**
  * @brief  Checks whether the specified HSPIS interrupt has occurred or not.
  * @retval The new state of HSPIS_IT (SET or RESET).
  */
ITStatus HSPIS_GetITStatus(HSPIS_TypeDef *HSPIx, uint32_t HSPIS_IT)
{
	uint8_t itmask;
	
	assert_param(IS_HSPIM_GET_IT(HSPIS_IT));
	
	itmask = (uint8_t)((HSPIx->RISR << 4) | HSPIx->TISR);
	
	if ((itmask & HSPIS_IT) != RESET)
	{
		return SET;
	}	
	return RESET;
}

/**
  * @brief Reset HSPIS FIFO(TX or RX).
  */
void HSPIS_FIFOReset(HSPIS_TypeDef *HSPIx, uint32_t HSPIS_FIFO)
{
	HSPIx->FCR |= HSPIS_FIFO;
	
	HSPIx->FCR &= ~HSPIS_FIFO;
}


/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE**************************/
