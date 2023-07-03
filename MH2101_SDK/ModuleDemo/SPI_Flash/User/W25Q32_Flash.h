#ifndef _W25Q32_FLASH_H_
#define _W25Q32_FLASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mhscpu.h"

#define W25Q32_FLASH_BASE                (0x0UL)
#define W25Q32_FLASH_SIZE	             (0x400000)
#define W25Q32_PAGE_SIZE                 (0x100)
#define W25Q32_SECTOR_SIZE               (0x1000)
#define W25Q32_PAGE_NUM                  (W25Q32_FLASH_SIZE/W25Q32_PAGE_SIZE) 	
	
#define W25Q32_CMD_WRITE_ENABLE          0x06	
#define W25Q32_CMD_WRITE_DISABLE         0x04
#define W25Q32_CMD_READ_IDENTIFICATION   0x9F
#define W25Q32_CMD_READ_STATUS           0x05	
#define W25Q32_CMD_READ_DATA             0x03
#define W25Q32_CMD_FAST_READ_DATA        0x0B
#define W25Q32_CMD_PAGE_PROGRAM          0x02
#define W25Q32_CMD_CHIP_ERASE            0xC7
#define W25Q32_CMD_SECTOR_ERASE          0x20	
#define W25Q32_CMD_POWER_DOWN            0xB9
#define W25Q32_CMD_RELEASE_POWER_DOWN    0xAB	

#define Dummy_Byte                       0xA5

#define SPI_FLASH_GROUP                  SPIM2
#define SPI_FLASH_CS_PORT                GPIOE
#define SPI_FLASH_CS_PIN                 GPIO_Pin_6

#define W25Q32_FLASH_CS_LOW()            GPIO_ResetBits(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN)
#define W25Q32_FLASH_CS_HIGH()           GPIO_SetBits(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN)

#define W25Q32_FLASH_STATUS_WIP          (1UL << 0)
#define W25Q32_FLASH_STATUS_WEL          (1UL << 1)


typedef enum
{
	OPERATE_SUCCESS = 0,
	OPERATE_BAD_PARAM,
	OPERATE_FAILED
}FLASH_RESULT_T;

void W25Q32_FlashInit(void);

uint32_t W25Q32_FlashReadID(void);

void W25Q32_FlashWriteEnable(void);
void W25Q32_FlashWriteDisable(void);

uint32_t W25Q32_FlashReadStatus(void);

int W25Q32_FlashReadData(uint32_t addr, uint8_t *buffer, uint32_t size);
int W25Q32_FlashReadDataHS(uint32_t addr, uint8_t *buffer, uint32_t size);

int W25Q32_FlashPageProgram(uint32_t addr, uint8_t *buffer, uint32_t size);

int W25Q32_FlashChipErase(void);
int W25Q32_FlashSecrorErase(uint32_t addr);

void W25Q32_FlashPowerDown(void);
void W25Q32_FlashReleasePowerDown(void);


#ifdef __cplusplus
}
#endif


#endif   ///< _W25Q32_FLASH_H_
