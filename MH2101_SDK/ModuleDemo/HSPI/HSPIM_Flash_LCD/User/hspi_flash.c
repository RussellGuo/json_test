/************************ (C) COPYRIGHT Megahuntmicro ********************************************************
 * @file                : hspi_flash.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 8-November-2019
 *************************************************************************************************************/
 
#include "hspi_flash.h"

/** @defgroup HSPI_Flash_Private_Defines
  * @{
  */
#define QF_SIZE									(0x1000000)									
#define QF_SECTOR_SIZE							(0x1000)
#define QF_PAGE_SIZE							(0x100)
#define QF_SECTOR_NUM							(QF_SIZE/QF_SECTOR_SIZE)
#define QF_PAGE_NUM								(QF_SIZE/QF_PAGE_SIZE)
	
#define W25X_WRITE_ENABLE						(0x06)
#define W25X_WRITE_DISABLE						(0x04)
#define W25X_READ_STATUS_REG					(0x05)
#define W25X_WRITE_STATUS_REG					(0x01)
#define W25X_READ_DATA							(0x03)
#define W25X_PAGE_PROGRAM						(0x02)
#define W25X_BLOCK_ERASE						(0xD8)
#define W25X_SECTOR_ERASE						(0x20)
#define W25X_CHIP_ERASE							(0xC7)
	
#define W25X_WIP_FLAG							(0x01)
#define W25X_DUMMY_BYTE							(0xFF)
extern HSPIM_TypeDef *HSPIx;
/**
  * @}
  */
			
uint32_t QF_ReadID(void)
{
	uint32_t temp = 0;
	uint32_t temp0 = 0; 
	uint32_t temp1 = 0; 
	uint32_t temp2 = 0;
	
	QF_CS_LOW();
	QF_ReadWriteByte(0x9F);
	temp0 = QF_ReadWriteByte(0x00);
	temp1 = QF_ReadWriteByte(0x00);
	temp2 = QF_ReadWriteByte(0x00);
	QF_CS_HIGH();
	
	temp = (temp0 << 16) | (temp1 << 8) | temp2;
	return temp;
}

uint8_t QF_ReadWriteByte(uint8_t WriteData)
{ 
	HSPIM_SendData(HSPIx,WriteData);
	while(HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_TXE) == RESET); 
	
	while(HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_RXE) == SET);  
	return HSPIM_ReceiveData(HSPIx);
}

void QF_WriteEnable(void)
{
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_WRITE_ENABLE);
	QF_CS_HIGH();
}

uint8_t QF_ReadSR(void)
{
	uint8_t temp = 0;
	
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_READ_STATUS_REG);
	temp = QF_ReadWriteByte(W25X_DUMMY_BYTE);
	QF_CS_HIGH();
	
	return temp;
	
}
void QF_WaitBusy(void)
{
	while((W25X_WIP_FLAG & QF_ReadSR()) == SET);
}

void QF_EraseChip(void)
{
	QF_WriteEnable();
	QF_WaitBusy();
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_CHIP_ERASE);
	QF_CS_HIGH();
	QF_WaitBusy();
}

void QF_EraseSector(uint32_t Addr)
{
	QF_WriteEnable();
	QF_WaitBusy();
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_SECTOR_ERASE);
	QF_ReadWriteByte((Addr & 0xFF0000) >> 16);
	QF_ReadWriteByte((Addr & 0xFF00) >> 8);
	QF_ReadWriteByte(Addr & 0xFF);
	QF_CS_HIGH();
	QF_WaitBusy();
}

void QF_WritePage(uint8_t* pBuff, uint32_t Addr, uint32_t Num)
{
	QF_WriteEnable();
	QF_WaitBusy();
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_PAGE_PROGRAM);
	QF_ReadWriteByte((Addr & 0xFF0000) >> 16);
	QF_ReadWriteByte((Addr & 0xFF00) >> 8);
	QF_ReadWriteByte(Addr & 0xFF);

	if (Num > QF_PAGE_NUM)
	{
		Num = QF_PAGE_NUM;
	}

	while(Num--)
	{
		QF_ReadWriteByte(*pBuff);
		pBuff++;
	}

	QF_CS_HIGH();
	QF_WaitBusy();
}


void QF_WriteBuff(uint8_t* pBuff, uint32_t Addr, uint32_t Num)
{
	uint8_t addr = 0; 
	uint8_t temp = 0;
	uint8_t count = 0; 
	uint8_t numpage = 0; 
	uint8_t numsingle = 0; 
	
	addr = Addr % QF_PAGE_SIZE;
	count = QF_PAGE_SIZE - addr;
	numpage = Num / QF_PAGE_SIZE;
	numsingle = Num / QF_PAGE_SIZE;
	
	if (0 == addr)
	{
		if (0 == numpage)
		{
			QF_WritePage(pBuff, Addr, Num);
		}
		else 
		{
			while(numpage--)
			{
				QF_WritePage(pBuff, Addr, QF_PAGE_SIZE);
				Addr += QF_PAGE_SIZE;
				pBuff += QF_PAGE_SIZE;			
			}
			
			QF_WritePage(pBuff, Addr, numsingle);	
		}
	}
	else 
	{		
		if (0 == numpage)
		{
			if (numsingle > count)
			{
				temp = numsingle - count;
				QF_WritePage(pBuff, Addr, count);
				Addr += count;
				pBuff += count;	
				QF_WritePage(pBuff, Addr, temp);	
			}
			else 
			{
				QF_WritePage(pBuff, Addr, Num);
			}
		}
		else 
		{
			Num -= count;
			numpage = Num / QF_PAGE_SIZE;
			numsingle = Num / QF_PAGE_SIZE;
			QF_WritePage(pBuff, Addr, count);
			Addr += count;
			pBuff += count;	
			
			while(numpage--)
			{
				QF_WritePage(pBuff, Addr, QF_PAGE_SIZE);
				Addr += QF_PAGE_SIZE;
				pBuff += QF_PAGE_SIZE;			
			}
			
			if (0 != numsingle)
			{
				QF_WritePage(pBuff, Addr, numsingle);
			}
		}
	}
}

void QF_ReadBuff(uint8_t* pBuff, uint32_t Addr, uint32_t Num)
{
	QF_CS_LOW();
	QF_ReadWriteByte(W25X_READ_DATA);
	QF_ReadWriteByte((Addr & 0xFF0000) >> 16);
	QF_ReadWriteByte((Addr & 0xFF00) >> 8);
	QF_ReadWriteByte(Addr & 0xFF);
	while(Num--)
	{
		*pBuff = QF_ReadWriteByte(W25X_DUMMY_BYTE);
		pBuff++;
	}
	QF_CS_HIGH();
}

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE******************************************************************/
