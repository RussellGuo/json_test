#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
 


void UART_Configuration(void);
void clearBPK(void);
void setBPK(void);
void printBPK(void);

void SENSOR_DisableAll(void)
{
	SENSOR_EXTCmd(DISABLE);
	while (RESET != SENSOR_EXTIsRuning());		
	SENSOR_ClearITPendingBit();
	
	SENSOR_ANACmd(SENSOR_ANA_VOL_HIGH | SENSOR_ANA_VOL_LOW | SENSOR_ANA_TEMPER_HIGH | SENSOR_ANA_TEMPER_LOW
	              | SENSOR_ANA_XTAL32K | SENSOR_ANA_MESH | SENSOR_ANA_VOLGLITCH, DISABLE);
}  
  
void BPK_ReadLockFun(void)
{
	printf("%s\n", __func__);
    BPK_KeyReadLock(BPK_KEY_REGION_0 | BPK_KEY_REGION_1, ENABLE);
}

void BPK_ReadUnlockFun(void)
{
	printf("%s\n", __func__);
    BPK_KeyReadLock(BPK_KEY_REGION_0 | BPK_KEY_REGION_1, DISABLE);
}

void BPK_WriteLockFun(void)
{
	printf("%s\n", __func__);
	BPK_KeyWriteLock(BPK_KEY_REGION_0 | BPK_KEY_REGION_1, ENABLE);
}

void BPK_WriteUnlockFun(void)
{
	printf("%s\n", __func__);
	BPK_KeyWriteLock(BPK_KEY_REGION_0 | BPK_KEY_REGION_1, DISABLE);
}

void BPK_ReadLock_Test(void)
{
	printf("=======================%s=======================\n", __func__);
	printf("write and read lock disable\n");
	BPK_ReadUnlockFun();
	BPK_WriteUnlockFun();
	clearBPK();
    setBPK();
	printf("read lock enable\n");
	BPK_ReadLockFun();
	printBPK();
}

void BPK_WriteLock_Test(void)
{
	printf("=======================%s=======================\n", __func__);
	printf("write and read lock disable\n");
	BPK_ReadUnlockFun();
	BPK_WriteUnlockFun();
	clearBPK();
    setBPK();
	printf("write lock enable\n");
	BPK_WriteLockFun();
	clearBPK();
}

void BPK_KeyClean_Test(void)
{
	printf("=======================%s=======================\n", __func__);
	printf("write and read lock disable\n");
	BPK_ReadUnlockFun();
	BPK_WriteUnlockFun();
    setBPK();
	printf("clean region 0\n");	
    BPK_KeyClear(BPK_KEY_REGION_0);
    printBPK();
	printf("clean region 1\n");		
    BPK_KeyClear(BPK_KEY_REGION_1);
    printBPK();
}


void BPK_Lock_Test(void)
{
	printf("=======================%s=======================\n", __func__);
	printf("--------Lock WriteLock ReadLock Test-------\n");
	BPK_ReadLockFun();
	BPK_WriteLockFun();
	printf("BPK_LRA = %08X\n", BPK->BPK_LRA);
	printf("BPK_LWA = %08X\n", BPK->BPK_LWA);
	
	printf("\nLock WriteLock ReadLock\n\n");
	BPK_Lock(BPK_LR_LOCK_KEYWRITE | BPK_LR_LOCK_KEYREAD, ENABLE);
	BPK_ReadUnlockFun();
	BPK_WriteUnlockFun();
	printf("BPK_LRA = %08X\n", BPK->BPK_LRA);
	printf("BPK_LWA = %08X\n", BPK->BPK_LWA);
	
	printf("\nUnlock WriteLock ReadLock\n\n");
	BPK_Lock(BPK_LR_LOCK_KEYWRITE | BPK_LR_LOCK_KEYREAD, DISABLE);
	BPK_ReadUnlockFun();
	BPK_WriteUnlockFun();
	printf("BPK_LRA = %08X\n", BPK->BPK_LRA);
	printf("BPK_LWA = %08X\n", BPK->BPK_LWA);
	
	printf("\n-----------Lock KeyClear Test-------------\n");
	printf("\nLock KeyClear\n\n");
	BPK_Lock(BPK_LR_LOCK_KEYCLEAR, ENABLE);
	BPK_KeyClean_Test();
	
	printf("\nUnlock KeyClear\n\n");
	BPK_Lock(BPK_LR_LOCK_KEYCLEAR, DISABLE);
	BPK_KeyClean_Test();
	
	printf("\n-----------Lock SetScramber Test----------\n");
	setBPK();
	printf("Lock SetScramber Enable\n\n");
	BPK_Lock(BPK_LR_LOCK_SCRAMBER, ENABLE);
	printf("BPK SetScramber Exe\n");
	BPK_SetScramber(0x12345678);
	printBPK();
	
	printf("Lock SetScramber Disable\n\n");
	BPK_Lock(BPK_LR_LOCK_SCRAMBER, DISABLE);
	printf("BPK SetScramber Exe\n");	
	BPK_SetScramber(0x12345678);
	printBPK();
	
	printf("\n--------------Lock Reset Test-------------\n");
	setBPK();
    BPK_KeyReadLock(BPK_KEY_REGION_0, ENABLE);
    BPK_KeyReadLock(BPK_KEY_REGION_1, ENABLE);
    BPK_KeyWriteLock(BPK_KEY_REGION_0, ENABLE);
    BPK_KeyWriteLock(BPK_KEY_REGION_1, ENABLE);
	printf("BPK_LRA = %08X\n", BPK->BPK_LRA);
	printf("BPK_LWA = %08X\n", BPK->BPK_LWA);
	printf("SEN_EXTS_START = %08X\n", SENSOR->SEN_EXTS_START);
	
	SENSOR_DisableAll();
}

void BPK_LockLock_Test(void)
{
	printf("=======================%s=======================\n", __func__);
	printf("lock disable\n");
	BPK_Lock(BPK_LR_LOCK_KEYWRITE | BPK_LR_LOCK_KEYREAD |
			 BPK_LR_LOCK_RESET | BPK_LR_LOCK_KEYCLEAR | BPK_LR_LOCK_SCRAMBER, DISABLE);
	printf("BPK->BPK_LR = %08X\n", BPK->BPK_LR);
	
	printf("lock enable\n");
	BPK_Lock(BPK_LR_LOCK_KEYWRITE | BPK_LR_LOCK_KEYREAD |
			 BPK_LR_LOCK_RESET | BPK_LR_LOCK_KEYCLEAR | BPK_LR_LOCK_SCRAMBER, ENABLE);
	printf("BPK->BPK_LR = %08X\n", BPK->BPK_LR);
	
	printf("\nBPK_LockSelf Exe\n\n");
	BPK_LockSelf();
	printf("BPK->BPK_LR = %08X\n", BPK->BPK_LR);
	printf("lock disable\n");
	BPK_Lock(BPK_LR_LOCK_KEYWRITE | BPK_LR_LOCK_KEYREAD |
			 BPK_LR_LOCK_RESET | BPK_LR_LOCK_KEYCLEAR | BPK_LR_LOCK_SCRAMBER, DISABLE);
	printf("BPK->BPK_LR = %08X\n", BPK->BPK_LR);
	
	printf("\n\n*****************************************************\n");
	printf("After Lock BPK_LR, you need repower the battery!!!");
	printf("\n*****************************************************\n");	
}

int main(void)
{  
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU BPK Demo V1.0.\r\n");
	
	SENSOR_DisableAll();
	
	BPK_ReadLock_Test();
	BPK_WriteLock_Test();
	BPK_KeyClean_Test();
	BPK_Lock_Test();
	
	BPK_LockLock_Test();

	while(1)
	{
	}

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

void printBPK(void)
{
	uint32_t key[BPK_KEY_NUM];
	uint32_t index = 0;
	
	while (BPK_IsReady() == RESET);

	printf("%s\n", __func__);
	memset(key, 0, sizeof(key));
	BPK_ReadKey(key, sizeof(key)/sizeof(key[0]), 0);
	
	for (index = 0; index < sizeof(key)/sizeof(key[0]); index++)
	{
		printf("%08X ", key[index]);
		if (3 == index % 4)
		{
			printf("\n");
		}
	}
	printf("\n");
	memset(key, 0, sizeof(key));
}

void clearBPK(void)
{
	uint32_t buf[BPK_KEY_NUM];
	
	printf("%s\n", __func__);
	while (BPK_IsReady() == RESET);
	
	// clear buf
	memset(buf, 0, sizeof(buf));
	BPK_WriteKey(buf, sizeof(buf)/sizeof(buf[0]), 0);
	
	printBPK();
}

void setBPK(void)
{
	uint32_t buf[BPK_KEY_NUM];
	uint32_t index = 0;
	printf("%s\n", __func__);
	while (BPK_IsReady() == RESET);
	
	//set buf 0~X
	for (index = 0; index < sizeof(buf)/sizeof(buf[0]); index++)
	{
		buf[index] = index;
	}
	BPK_WriteKey(buf, sizeof(buf)/sizeof(buf[0]), 0);
	
	printBPK();
}


//Retarget Printf
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
