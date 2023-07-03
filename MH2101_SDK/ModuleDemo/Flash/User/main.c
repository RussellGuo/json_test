#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_it.h"


#define FLASH_SIZE_BYTES        (1024 * 1024 - 64 * 1024)

#define FLASH_START_ADDR 	    0x1010000U
#define FLASH_SECTOR_SIZE 	    (0x1000)
#define FLASH_SECTOR_NUM 	    (FLASH_SIZE_BYTES / FLASH_SECTOR_SIZE)
#define FLASH_PAGE_NUM 	        (FLASH_SECTOR_NUM * 16)         

#define FLASH_ARRAY_ADDR		0x1008000U


typedef enum
{
	DATA_TYPE_ALL_ZERO = 0x0,
	DATA_TYPE_ALL_ONE,
	DATA_TYPE_A5A5A5A5,
	DATA_TYPE_ADDRESS_SELF,
    DATA_TYPE_00TOFF,
}FLASH_TEST_DATA_TYPE;


void UART_Configuration(void);

static void DataPrintf(void *buf, uint32_t bufsize);
static int DataCheck(void *src, void *dst, uint32_t size);
static int DataCheckReverse(void *src, void *dst, uint32_t size);
static int EraseCheck(uint32_t addr, uint32_t pagNum);
static int FlashTest(uint32_t pagNum, uint32_t dataType);


int main(void)
{
	uint32_t i = 0;

    QSPI_Init(NULL);   
 
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
    
    QSPI_SetLatency(0);
	
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);	
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);
    
	UART_Configuration();
	printf("Megahunt Flash test V1.0....\n");
    
	while (1)
	{
		printf("\n>>>  Sector Erase, Program 0x00~0xFF\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector( FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		} 
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_00TOFF);    
		
		printf("\n>>>  Sector Erase, Program zero all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{ 
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ZERO);
		
		printf("\n>>>  Sector Erase, Program 0x00~0xFF\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_00TOFF);
		
		printf("\n>>>  Sector Erase, Program 0xA5A5A5A5 all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector( FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_A5A5A5A5);

		printf("\n>>>  Sector Erase, Program one all\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ONE);
		printf("\n>>>  No Erase, Program zero all\n");
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ALL_ZERO);

		printf("\n>>>  Sector Erase, Program address self\n");
		for (i = 0; i < FLASH_SECTOR_NUM; i++)
		{
			FLASH_EraseSector( FLASH_START_ADDR + i * FLASH_SECTOR_SIZE);
			EraseCheck(FLASH_START_ADDR + i * FLASH_SECTOR_SIZE, 16);
		}
		FlashTest(FLASH_PAGE_NUM, DATA_TYPE_ADDRESS_SELF);
		
		printf("\Flash test end\n");		
	}
	
}

static void DataPrintf(void *buf, uint32_t bufsize)
{
	uint32_t i = 0;
	uint8_t *pBuf = (uint8_t *)buf;
	
	if (0 != bufsize)
	{
		for (i = 0; i < bufsize; i++)
		{
			if (0 != i && 0 == i%16)
			{
				printf(" \n");			
			}

			printf("%02X ", pBuf[i]);
		}
	}
	printf("\n");
}

static int DataCheck(void *src, void *dst, uint32_t size)
{
	uint8_t *pSrc = (uint8_t *)src, *pDst = (uint8_t *)dst;
	
	if (memcmp(pDst, pSrc, size))
	{
		DataPrintf(pDst, size);
		return -1;
	}

	return 0;
}

static int DataCheckReverse(void *src, void *dst, uint32_t size)
{
	uint32_t i;
	uint8_t *pSrc = (uint8_t *)src, *pDst = (uint8_t *)dst;

	for (i = size - 1; i > 0; i--)
	{
		if (pSrc[i] != pDst[i])
		{
			return -1;
		}
	}
	
	return 0;
}

static int EraseCheck(uint32_t addr, uint32_t pagNum)
{
	uint32_t i = 0;
	uint8_t erase_Buf[FLASH_PAGE_SIZE];

	memset(erase_Buf, 0xFF, FLASH_PAGE_SIZE);	
 
	CACHE_CleanAll(CACHE);
	for (i = 0; i < pagNum; i++)
	{	
		if (-1 == DataCheck(erase_Buf, (uint8_t *)(addr + i * FLASH_PAGE_SIZE), sizeof(erase_Buf)))
		{
			printf("Erase Check failed!\n");
            while(1);
		}
	}	
	
	return 0;
}

static int FlashTest(uint32_t pagNum, uint32_t dataType)
{
	uint32_t i = 0, j = 0;
	uint32_t write_Buf[FLASH_PAGE_SIZE/4] = {0};
    uint8_t data_buf_00toFF[FLASH_PAGE_SIZE] = {0};
	uint32_t program_addr = FLASH_START_ADDR;
	
    for (i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        data_buf_00toFF[i] = i;
    }
	
	switch (dataType)
	{
		case DATA_TYPE_ALL_ONE:
			memset(write_Buf, 0xFF, sizeof(write_Buf));			
			break;
	
		case DATA_TYPE_A5A5A5A5:
			memset(write_Buf, 0xA5, sizeof(write_Buf));
			break;
		
        case DATA_TYPE_00TOFF:
			memset(write_Buf, 0x00, sizeof(write_Buf));
            memcpy(write_Buf, data_buf_00toFF, sizeof(data_buf_00toFF));
            break;
        
		case DATA_TYPE_ADDRESS_SELF:
			break;
			
		case DATA_TYPE_ALL_ZERO:
		default:			
			memset(write_Buf, 0x00, sizeof(write_Buf));
			break;
	}

	printf("Flash Programe data: \n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < FLASH_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * FLASH_PAGE_SIZE + j;
			}
		}

		FLASH_ProgramPage(program_addr + i * FLASH_PAGE_SIZE, sizeof(write_Buf), (uint8_t*)(write_Buf));	
	}

	printf("Flash Read Data Start\n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < FLASH_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * FLASH_PAGE_SIZE + j;
			}
		}
		
		CACHE_CleanAll(CACHE);
		if (-1 == DataCheck(write_Buf, (uint8_t *)(program_addr + i * FLASH_PAGE_SIZE), sizeof(write_Buf)))
		{
			printf("Addr %#x data[Cache] check error!\n", program_addr + i * FLASH_PAGE_SIZE);
            while(1);
		}
	}	
	
	printf("Flash Read Data Check OK \n");	
	return 0;
}


void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);
}


int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);

	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
