#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void UART_Configuration(void);

  
int main(void)
{
	uint8_t str[] = "123456789";
	uint32_t result;
	
	CRC_ConfigTypeDef CRC_16_Config;
	CRC_ConfigTypeDef CRC_16_Modbus_Config;
	CRC_ConfigTypeDef CRC_CCITT_0xffff_Config;
	CRC_ConfigTypeDef CRC_CCITT_XModem_Config;
	CRC_ConfigTypeDef CRC_32_Config;
	
	CRC_16_Config.CRC_Poly = CRC_Poly_16_8005;
	CRC_16_Config.CRC_PolyMode = CRC_PolyMode_Reversed;
	CRC_16_Config.CRC_Init_Value = 0;
	CRC_16_Config.CRC_Xor_Value = 0;
	
	CRC_16_Modbus_Config.CRC_Poly = CRC_Poly_16_8005;
	CRC_16_Modbus_Config.CRC_PolyMode = CRC_PolyMode_Reversed;
	CRC_16_Modbus_Config.CRC_Init_Value = 0xFFFF;
	CRC_16_Modbus_Config.CRC_Xor_Value = 0;
	
	CRC_CCITT_0xffff_Config.CRC_Poly = CRC_Poly_16_1021;
	CRC_CCITT_0xffff_Config.CRC_PolyMode = CRC_PolyMode_Normal;
	CRC_CCITT_0xffff_Config.CRC_Init_Value = 0xFFFF;
	CRC_CCITT_0xffff_Config.CRC_Xor_Value = 0;
	
	CRC_CCITT_XModem_Config.CRC_Poly = CRC_Poly_16_1021;
	CRC_CCITT_XModem_Config.CRC_PolyMode = CRC_PolyMode_Normal;
	CRC_CCITT_XModem_Config.CRC_Init_Value = 0;
	CRC_CCITT_XModem_Config.CRC_Xor_Value = 0;
	
	CRC_32_Config.CRC_Poly = CRC_Poly_32_04C11DB7;
	CRC_32_Config.CRC_PolyMode = CRC_PolyMode_Reversed;
	CRC_32_Config.CRC_Init_Value = 0xFFFFFFFF;
	CRC_32_Config.CRC_Xor_Value = 0xFFFFFFFF;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_CRC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_CRC, ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU CRC Demo V1.0.\r\n");
	
	printf("string for CRC caculate : %s \r\n",str);
	
	result = CRC_CalcBlockCRC(CRC_16,str,sizeof(str)-1);
	printf("CRC_16 :            		%08X \r\n",result);
	result = CRC_Calc(&CRC_16_Config,str,sizeof(str)-1);
	printf("CRC_16_Config :     		%08X \r\n",result);
	
	result = CRC_CalcBlockCRC(CRC_16_Modbus,str,sizeof(str)-1);
	printf("CRC_16_Modbus :     		%08X \r\n",result);
	result = CRC_Calc(&CRC_16_Modbus_Config,str,sizeof(str)-1);
	printf("CRC_16_Modbus_Config :		%08X \r\n",result);
	
	result = CRC_CalcBlockCRC(CRC_CCITT_0xffff,str,sizeof(str)-1);
	printf("CRC_CCITT_0xffff :  		%08X \r\n",result);
	result = CRC_Calc(&CRC_CCITT_0xffff_Config,str,sizeof(str)-1);
	printf("CRC_CCITT_0xffff_Config :  	%08X \r\n",result);
	
	result = CRC_CalcBlockCRC(CRC_CCITT_XModem,str,sizeof(str)-1);
	printf("CRC_CCITT_XModem :  		%08X \r\n",result);
	result = CRC_Calc(&CRC_CCITT_XModem_Config,str,sizeof(str)-1);
	printf("CRC_CCITT_XModem_Config :  	%08X \r\n",result);
	
	result = CRC_CalcBlockCRC(CRC_32,str,sizeof(str)-1);
	printf("CRC_32 :            		%08X \r\n",result);
	result = CRC_Calc(&CRC_32_Config,str,sizeof(str)-1);
	printf("CRC_32_Config :            	%08X \r\n",result);
	
	while(1);
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


//Retarget Printf
int SER_PutChar(int ch)
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
