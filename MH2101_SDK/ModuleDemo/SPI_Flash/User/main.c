#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_it.h"
#include "W25Q32_Flash.h"

void UART_Configuration(void);


int main(void)
{
	uint32_t i = 0;
	uint8_t readBuffer[W25Q32_PAGE_SIZE] = {0};
	uint8_t writeBuffer[W25Q32_PAGE_SIZE] = {0};
    uint8_t tmpBuffer[W25Q32_PAGE_SIZE] = {0};
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI2 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI2, ENABLE);

	UART_Configuration();
	printf("Megahunt SPI Flash Demo V1.0\n");
	
	W25Q32_FlashInit();
	
	for (i = 0; i < sizeof(writeBuffer); i++)
	{
		writeBuffer[i] = i;
	}
	
	printf("Flash ID:%#x\n", W25Q32_FlashReadID());
	
	printf("\nErase All:");
	W25Q32_FlashChipErase();
    memset(tmpBuffer, 0xFF, sizeof(tmpBuffer));
    for (i = 0; i < W25Q32_PAGE_NUM; i++)
	{
        W25Q32_FlashReadData(W25Q32_FLASH_BASE+i*W25Q32_PAGE_SIZE, readBuffer, sizeof(readBuffer));
		if (memcmp(readBuffer, tmpBuffer, sizeof(readBuffer)))
        {
            printf("Chip Erase error.\n");
            while(1);
        }
        printf("addr %#lx erase ok.\n", W25Q32_FLASH_BASE+i*W25Q32_PAGE_SIZE);
	}
    printf("\nErase done.\n");
	
	printf("\nWrite:");
	W25Q32_FlashPageProgram(0x10000, writeBuffer, sizeof(writeBuffer));
	W25Q32_FlashReadDataHS(0x10000, readBuffer, sizeof(readBuffer));	
    if (memcmp(readBuffer, writeBuffer, sizeof(readBuffer)))
    {
        printf("Chip write error.\n");
        while(1);
    }
		
	printf("\nPower Down:");
	W25Q32_FlashPowerDown();
	W25Q32_FlashReadDataHS(0x10000, readBuffer, sizeof(readBuffer));
    memset(tmpBuffer, 0x00, sizeof(tmpBuffer));
    if (memcmp(readBuffer, tmpBuffer, sizeof(readBuffer)))
    {
        printf("Chip Power Down error.\n");
    }
	
	printf("\nRelease from Power Down:");
	W25Q32_FlashReleasePowerDown();
	W25Q32_FlashReadDataHS(0x10000, readBuffer, sizeof(readBuffer));
    if (memcmp(readBuffer, writeBuffer, sizeof(readBuffer)))
    {
        printf("Chip Release from Power Down error.\n");
        while(1);
    }
	
	printf("\nProgram All:");
	W25Q32_FlashChipErase();
	for (i = 0; i < W25Q32_PAGE_NUM; i++)
	{
		W25Q32_FlashPageProgram(W25Q32_FLASH_BASE+i*W25Q32_PAGE_SIZE, writeBuffer, sizeof(writeBuffer));
		memset(readBuffer, 0, sizeof(readBuffer));
		W25Q32_FlashReadData(W25Q32_FLASH_BASE+i*W25Q32_PAGE_SIZE, readBuffer, sizeof(readBuffer));
        if (memcmp(readBuffer, writeBuffer, sizeof(readBuffer)))
        {
            printf("Chip program error.\n");
            while(1);
        }
        printf("addr %#lx program ok.\n", W25Q32_FLASH_BASE+i*W25Q32_PAGE_SIZE);
	}
	
    printf("test pass.\n");
	while (1)
	{
	
	}
	
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

