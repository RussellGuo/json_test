#include <stdio.h>
#include "W25Q32_Flash.h"

//#define W25Q32_FLASH_DEBUG

#ifdef W25Q32_FLASH_DEBUG
void hexdump(uint32_t addr, const void *data, uint32_t len, uint32_t lineLen)
{
	uint32_t i;
	const uint8_t *buf = (const uint8_t *)data;
	
	for (i = 0; i < len; i++)
	{
		if (0 == i % lineLen)
		{
			printf("\n0x%08x: ", addr + i*lineLen);
		}
		
		printf("0x%02x ", buf[i]);
	}
		
	printf("\n");
}
#else
#define hexdump(addr, data, len, lineLen);
#endif

static uint8_t W25Q32_FlashSendBytes(uint8_t byte);

void W25Q32_FlashInit(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_0);
	
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_0;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStruct.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_0;
	
	SPI_Init(SPI_FLASH_GROUP, &SPI_InitStruct);
	SPI_Cmd(SPI_FLASH_GROUP, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = SPI_FLASH_CS_PIN;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	
	GPIO_Init(SPI_FLASH_CS_PORT, &GPIO_InitStruct);
	
	GPIO_SetBits(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN);
}

uint32_t W25Q32_FlashReadID(void)
{
	uint32_t temp0 = 0, temp1 = 0, temp2 = 0;
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_READ_IDENTIFICATION);
	temp0 = W25Q32_FlashSendBytes(Dummy_Byte);
	temp1 = W25Q32_FlashSendBytes(Dummy_Byte);
	temp2 = W25Q32_FlashSendBytes(Dummy_Byte);
	
	W25Q32_FLASH_CS_HIGH();	

	return (temp0 << 16) | (temp1 << 8) | temp2;
}

void W25Q32_FlashWriteEnable(void)
{
	W25Q32_FLASH_CS_LOW();
	W25Q32_FlashSendBytes(W25Q32_CMD_WRITE_ENABLE);
	W25Q32_FLASH_CS_HIGH();	
}

void W25Q32_FlashWriteDisable(void)
{
	W25Q32_FLASH_CS_LOW();
	W25Q32_FlashSendBytes(W25Q32_CMD_WRITE_DISABLE);
	W25Q32_FLASH_CS_HIGH();	
}

uint32_t W25Q32_FlashReadStatus(void)
{
	uint8_t status = 0;
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_READ_STATUS);
	status = W25Q32_FlashSendBytes(Dummy_Byte);
	
	W25Q32_FLASH_CS_HIGH();
	
	return status;
}

int W25Q32_FlashReadData(uint32_t addr, uint8_t *buffer, uint32_t size)
{
	int count = 0;
	
	if (addr >= W25Q32_FLASH_SIZE || NULL == buffer || size < 1)
	{
		return OPERATE_BAD_PARAM;
	}
	
	memset(buffer, 0, size);
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_READ_DATA);
	W25Q32_FlashSendBytes((addr >> 16) & 0xFF);
	W25Q32_FlashSendBytes((addr >> 8) & 0xFF);
	W25Q32_FlashSendBytes(addr & 0xFF);
	
	for (count = 0; count < size; count++)
	{
		buffer[count] = W25Q32_FlashSendBytes(Dummy_Byte);
	}
	
	W25Q32_FLASH_CS_HIGH();
	
	hexdump(addr, buffer, size, 16);
	
	return count;
}

int W25Q32_FlashReadDataHS(uint32_t addr, uint8_t *buffer, uint32_t size)
{
	int count = 0;
	
	if (addr >= W25Q32_FLASH_SIZE || NULL == buffer || size < 1)
	{
		return OPERATE_BAD_PARAM;
	}
	
	memset(buffer, 0, size);
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_FAST_READ_DATA);
	W25Q32_FlashSendBytes((addr >> 16) & 0xFF);
	W25Q32_FlashSendBytes((addr >> 8) & 0xFF);
	W25Q32_FlashSendBytes(addr & 0xFF);
	W25Q32_FlashSendBytes(Dummy_Byte);
	
	for (count = 0; count < size; count++)
	{
		buffer[count] = W25Q32_FlashSendBytes(Dummy_Byte);
	}
	
	W25Q32_FLASH_CS_HIGH();
	
	hexdump(addr, buffer, size, 16);	
	
	return count;
}

int W25Q32_FlashPageProgram(uint32_t addr, uint8_t *buffer, uint32_t size)
{
	int count = 0;
	
	if (addr >= W25Q32_FLASH_SIZE || NULL == buffer || size < 1)
	{
		return OPERATE_BAD_PARAM;
	}
	
	W25Q32_FlashWriteEnable();
	if (!(W25Q32_FLASH_STATUS_WEL & W25Q32_FlashReadStatus()))
	{
		return OPERATE_FAILED;
	}
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_PAGE_PROGRAM);
	W25Q32_FlashSendBytes((addr >> 16) & 0xFF);
	W25Q32_FlashSendBytes((addr >> 8) & 0xFF);
	W25Q32_FlashSendBytes(addr & 0xFF);
	
	for (count = 0; count < size; count++)
	{
		W25Q32_FlashSendBytes(buffer[count]);
	}
	
	W25Q32_FLASH_CS_HIGH();
	
	while (W25Q32_FlashReadStatus() & W25Q32_FLASH_STATUS_WIP);
	
	return count;
}

int W25Q32_FlashChipErase(void)
{
	W25Q32_FlashWriteEnable();
	if (!(W25Q32_FLASH_STATUS_WEL & W25Q32_FlashReadStatus()))
	{
		return OPERATE_FAILED;
	}
	
	W25Q32_FLASH_CS_LOW();
	W25Q32_FlashSendBytes(W25Q32_CMD_CHIP_ERASE);
	W25Q32_FLASH_CS_HIGH();

	while (W25Q32_FlashReadStatus() & W25Q32_FLASH_STATUS_WIP);
	
	return OPERATE_SUCCESS;
}

int W25Q32_FlashSecrorErase(uint32_t addr)
{
	if (addr >= W25Q32_FLASH_SIZE)
	{
		return OPERATE_BAD_PARAM;
	}
	
	W25Q32_FlashWriteEnable();
	if (W25Q32_FLASH_STATUS_WEL != W25Q32_FlashReadStatus())
	{
		return OPERATE_FAILED;
	}
	
	W25Q32_FLASH_CS_LOW();
	
	W25Q32_FlashSendBytes(W25Q32_CMD_SECTOR_ERASE);
	W25Q32_FlashSendBytes((addr >> 16) & 0xFF);
	W25Q32_FlashSendBytes((addr >> 8) & 0xFF);
	W25Q32_FlashSendBytes(addr & 0xFF);
	
	W25Q32_FLASH_CS_HIGH();

	while (W25Q32_FlashReadStatus() & W25Q32_FLASH_STATUS_WIP);
	
	return OPERATE_SUCCESS;
}

void W25Q32_FlashPowerDown(void)
{
	W25Q32_FLASH_CS_LOW();
	W25Q32_FlashSendBytes(W25Q32_CMD_POWER_DOWN);	
	W25Q32_FLASH_CS_HIGH();
}

void W25Q32_FlashReleasePowerDown(void)
{
	W25Q32_FLASH_CS_LOW();
	W25Q32_FlashSendBytes(W25Q32_CMD_RELEASE_POWER_DOWN);	
	W25Q32_FLASH_CS_HIGH();
}


static uint8_t W25Q32_FlashSendBytes(uint8_t byte)
{
	while (RESET == SPI_GetFlagStatus(SPI_FLASH_GROUP, SPI_FLAG_TXE));
	SPI_SendData(SPI_FLASH_GROUP, byte);
	
	while (RESET == SPI_GetFlagStatus(SPI_FLASH_GROUP, SPI_FLAG_RXNE));
	return SPI_ReceiveData(SPI_FLASH_GROUP);
}
