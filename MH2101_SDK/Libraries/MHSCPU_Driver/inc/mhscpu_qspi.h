/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_qspi.h
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file contains all the functions prototypes for the QSPI firmware library
 *****************************************************************************/
 
#ifndef MHSMCU_FLASH
#define MHSMCU_FLASH

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "mhscpu.h"


/* Size of the flash */
#define FLASH_PAGE_SIZE				        0x100    

// Register Operations 
#define READ_STATUS_REG1_CMD			    0x05

// Program Operations 
#define QUAD_INPUT_PAGE_PROG_CMD            0x32

/***End Cmd***/

#define QSPI_DEVICE_PARA_SAMPLE_DLY_Pos       15
#define QSPI_DEVICE_PARA_SAMPLE_PHA_Pos       14
#define QSPI_DEVICE_PARA_PROTOCOL_Pos         8
#define QSPI_DEVICE_PARA_DUMMY_CYCLE_Pos      4

#define	QSPI_BUSMODE_111                         0x00	//CMD-ADDR-DATA = 1-1-1
#define	QSPI_BUSMODE_114                         0x01	//CMD-ADDR-DATA = 1-1-4
#define	QSPI_BUSMODE_144                         0x02	//CMD-ADDR-DATA = 1-4-4
#define	QSPI_BUSMODE_444                         0x03	//CMD-ADDR-DATA = 4-4-4



#define QSPI_CMDFORMAT_CMD8					     0x00
#define	QSPI_CMDFORMAT_CMD8_RREG8			     0x01
#define	QSPI_CMDFORMAT_CMD8_RREG16			     0x02
#define	QSPI_CMDFORMAT_CMD8_RREG24			     0x03
#define	QSPI_CMDFORMAT_CMD8_DMY24_WREG8		     0x04
#define	QSPI_CMDFORMAT_CMD8_ADDR24_RREG8	     0x05
#define	QSPI_CMDFORMAT_CMD8_ADDR24_RREG16	     0x06
#define	QSPI_CMDFORMAT_CMD8_WREG8			     0x07
#define	QSPI_CMDFORMAT_CMD8_WREG16			     0x08
#define	QSPI_CMDFORMAT_CMD8_ADDR24			     0x09
#define	QSPI_CMDFORMAT_CMD8_ADDR24_RDAT		     0x0A
#define	QSPI_CMDFORMAT_CMD8_ADDR24_DMY_RDAT	     0x0B
#define	QSPI_CMDFORMAT_CMD8_ADDR24_M8_DMY_RDAT   0x0C
#define	QSPI_CMDFORMAT_CMD8_ADDR24_PDAT			 0x0D

#define ROM_QSPI_EraseChip                   (*((uint8_t (*)(QSPI_CommandTypeDef *))(*(uint32_t *)0x8020)))
#define ROM_QSPI_EraseSector                 (*((uint8_t (*)(QSPI_CommandTypeDef *, uint32_t))(*(uint32_t *)0x801C)))
#define ROM_QSPI_EraseBlock64k                (*((uint8_t (*)(QSPI_CommandTypeDef *, uint32_t))(*(uint32_t *)0x8070)))
#define ROM_QSPI_ProgramPage                 (*((uint8_t (*)(QSPI_CommandTypeDef *, DMA_TypeDef *, uint32_t, uint32_t, uint8_t *))(*(uint32_t *)0x8024)))

typedef enum
{
	QSPI_PROTOCOL_CLPL		= 0x00,
	QSPI_PROTOCOL_CHPH		= 0x03
}QSPI_ProtocolTypedef;

typedef enum
{
	QSPI_FREQSEL_HCLK_DIV2	= 0x01,
	QSPI_FREQSEL_HCLK_DIV3	= 0x02,
	QSPI_FREQSEL_HCLK_DIV4	= 0x03
}QSPI_FreqSelTypeDef;

typedef enum
{
	QSPI_STATUS_OK				=(0x00),
	QSPI_STATUS_ERROR			=(0x01),
	QSPI_STATUS_BUSY			=(0x02),
	QSPI_STATUS_NOT_SUPPORTED 	=(0x04),
	QSPI_STATUS_SUSPENDED		=(0x08)
} QSPI_StatusTypeDef;


typedef struct
{	
	//Device Para
	uint8_t SampleDly;						//Default:0
	uint8_t SamplePha;						//Default:0
	uint8_t ProToCol;						//Defualt: QSPI_PROTOCOL_CLPL
	uint8_t DummyCycles;					//Include M7:0  Defualt: 6
	uint8_t FreqSel;						//Defualt: QSPI_FREQSEL_HCLK_DIV4

	//Setting Cache
	uint8_t Cache_Cmd_ReleaseDeepInstruction;			//Defualt: 0xAB
	uint8_t Cache_Cmd_DeepInstruction;					//Defualt: 0xB9
	uint8_t Cache_Cmd_ReadBusMode;						//Defualt: QSPI_BUSMODE_144
	uint8_t Cache_Cmd_ReadFormat;						//Defualt: QSPI_CMDFORMAT_CMD8_ADDR24_DMY_RDAT
	uint8_t Cache_Cmd_ReadInstruction;					//Defualt: 0xEB
	
}QSPI_InitTypeDef;

typedef struct
{
	uint8_t  Instruction;       
	uint32_t BusMode;
	uint32_t CmdFormat;
	
}QSPI_CommandTypeDef;


#define ENABLE_CACHE_AES 		1
 

void QSPI_Init(QSPI_InitTypeDef *mhqspi);
void QSPI_SetLatency(uint32_t u32UsClk);

uint8_t FLASH_EraseChip(void);
uint8_t FLASH_EraseSector(uint32_t sectorAddress);
uint8_t FLASH_EraseBlock64k(uint32_t blockAddress);
uint8_t FLASH_ProgramPage(uint32_t addr, uint32_t size, uint8_t *buffer);


#ifdef __cplusplus
}
#endif

#endif


/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/
