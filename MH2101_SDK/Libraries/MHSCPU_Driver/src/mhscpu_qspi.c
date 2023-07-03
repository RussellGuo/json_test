/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_qspi.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file provides all the QSPI firmware functions
 *****************************************************************************/

 /* Includes ----------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_cache.h"
#include "mhscpu_qspi.h"
#include "mhscpu_dma.h"



void QSPI_Init(QSPI_InitTypeDef *mhqspi)
{
	if (mhqspi == NULL)
	{
		QSPI->DEVICE_PARA = ((QSPI->DEVICE_PARA & ~0xFF) | 0x6A) | QUADSPI_DEVICE_PARA_SAMPLE_PHA;
	}
	else
	{
		QSPI->CACHE_INTF_CMD = (uint32_t)((mhqspi->Cache_Cmd_ReleaseDeepInstruction << 24)
										 | (mhqspi->Cache_Cmd_DeepInstruction<<16)
										 | ((mhqspi->Cache_Cmd_ReadBusMode & 0x03) << 12)
									     | ((mhqspi->Cache_Cmd_ReadFormat & 0x0F) << 8) | (mhqspi->Cache_Cmd_ReadInstruction));
		QSPI->DEVICE_PARA  = (uint32_t)((QSPI->DEVICE_PARA & ~0xFFFF)
							 | ((mhqspi->SampleDly & 0x01) << QSPI_DEVICE_PARA_SAMPLE_DLY_Pos)
		                     | ((mhqspi->SamplePha & 0x01) << QSPI_DEVICE_PARA_SAMPLE_PHA_Pos)
							 | ((mhqspi->ProToCol & 0x03) << QSPI_DEVICE_PARA_PROTOCOL_Pos)
					         | ((mhqspi->DummyCycles & 0x0F) << QSPI_DEVICE_PARA_DUMMY_CYCLE_Pos)
							 | ((mhqspi->FreqSel & 0x03)
							 | QUADSPI_DEVICE_PARA_FLASH_READY));	
	}
}

/**
  * @brief  Sets the QSPI latency value.
  * @param  u32UsClk: specifies the QSPI Latency value.
  * @retval None
  */
void QSPI_SetLatency(uint32_t u32UsClk)
{
	SYSCTRL_ClocksTypeDef clocks;    
    
	if (0 == u32UsClk)
	{
        SYSCTRL_GetClocksFreq(&clocks);  
        QSPI->DEVICE_PARA = (QSPI->DEVICE_PARA & 0xFFFF) | ((clocks.CPU_Frequency*2/1000000) << 16);        
	}
	else
	{
        QSPI->DEVICE_PARA = (QSPI->DEVICE_PARA & 0xFFFF) | (u32UsClk << 16);
	}
}

/**
  * @brief  Flash Erase Chip.
  * @retval FLASH Status:  The returned value can be: QSPI_STATUS_ERROR, QSPI_STATUS_OK
  */
uint8_t FLASH_EraseChip(void)
{
    uint8_t ret;
    
	__disable_irq();
	__disable_fault_irq();	

    ret = ROM_QSPI_EraseChip(NULL);
    
	__enable_fault_irq();
	__enable_irq();

    return ret;
}

/**
  * @brief  Flash Erase Sector.
  * @param  sectorAddress: The sector address to be erased
  * @retval FLASH Status:  The returned value can be: QSPI_STATUS_ERROR, QSPI_STATUS_OK
  */
uint8_t FLASH_EraseSector(uint32_t sectorAddress)
{
    uint8_t ret;
    
	__disable_irq();
	__disable_fault_irq();	

    ret = ROM_QSPI_EraseSector(NULL, sectorAddress);
    
	__enable_fault_irq();
	__enable_irq();

    return ret;
}

/**
  * @brief  Flash Erase Sector.
  * @param  sectorAddress: The sector address to be erased
  * @retval FLASH Status:  The returned value can be: QSPI_STATUS_ERROR, QSPI_STATUS_OK
  */
uint8_t FLASH_EraseBlock64k(uint32_t blockAddress)
{
    uint8_t ret;
    
	__disable_irq();
	__disable_fault_irq();	

    ret = ROM_QSPI_EraseBlock64k(NULL, blockAddress);
    
	__enable_fault_irq();
	__enable_irq();

    return ret;
}


/**
  * @brief  Flash Program Interface.
  * @param  addr:          specifies the address to be programmed.
  * @param  size:          specifies the size to be programmed.
  * @param  buffer:        pointer to the data to be programmed, need word aligned
  * @retval FLASH Status:  The returned value can be: QSPI_STATUS_ERROR, QSPI_STATUS_OK
  */
uint8_t FLASH_ProgramPage(uint32_t addr, uint32_t size, uint8_t *buffer)
{
    uint8_t ret;
	QSPI_CommandTypeDef cmdType;
    
    cmdType.Instruction = QUAD_INPUT_PAGE_PROG_CMD;
    cmdType.BusMode = QSPI_BUSMODE_114;	      
    cmdType.CmdFormat = QSPI_CMDFORMAT_CMD8_ADDR24_PDAT;     
    
	__disable_irq();
	__disable_fault_irq();	

    ret = ROM_QSPI_ProgramPage(&cmdType, DMA_Channel_1, addr, size, buffer);
    
	__enable_fault_irq();
	__enable_irq();

    return ret;   
}




/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/
