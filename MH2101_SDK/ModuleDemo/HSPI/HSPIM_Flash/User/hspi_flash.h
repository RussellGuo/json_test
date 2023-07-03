/************************ (C) COPYRIGHT Megahuntmicro ********************************************************
 * @file                : hspi_flash.h
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 8-November-2019
 *************************************************************************************************************/
 
#ifndef __HSPI_FLASH_H
#define __HSPI_FLASH_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "mhscpu.h"
	
#define QF_CS_PORT								GPIOC	
#define QF_CS_PIN								GPIO_Pin_14
#define QF_CS_HIGH()							GPIO_SetBits(QF_CS_PORT, QF_CS_PIN)
#define QF_CS_LOW()								GPIO_ResetBits(QF_CS_PORT, QF_CS_PIN)

uint32_t QF_ReadID(void);	
uint8_t QF_ReadWriteByte(uint8_t WriteData);
void QF_WriteEnable(void);
void QF_WaitBusy(void);	
void QF_EraseChip(void);
void QF_EraseSector(uint32_t Address);
void QF_WritePage(uint8_t *pBuff, uint32_t Addr, uint32_t Num);	
void QF_WriteBuff(uint8_t* pBuff, uint32_t Addr, uint32_t Num);	
void QF_ReadBuff(uint8_t* pBuff, uint32_t Addr, uint32_t Num);	
	
#ifdef __cplusplus
}
#endif
#endif


/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE******************************************************************/
