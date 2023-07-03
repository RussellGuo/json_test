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


#define SPI_MODE_POLLING            1
#define SPI_MODE_INTERRUPT          2
#define SPI_MODE_SEL                SPI_MODE_POLLING    
  
#define DATA_BUF_SIZE				256

extern uint8_t rece_com;
extern uint8_t send_com;

uint32_t send_buf_index = 0;
uint32_t rece_buf_index = 0;
uint8_t data_buf[DATA_BUF_SIZE] = {0x00};

void NVIC_Configuration(void);
void SPI_Configuration(void);
void UART_Configuration(void);

static void DataPrintf(uint8_t *buf, uint32_t bufsize)
{
	uint32_t i = 0;
	if (0 != bufsize)
	{
		for(i = 0; i < bufsize; i++)
		{
			if(0 != i && 0 == i % 16)
				printf("\n");
			printf("%02X ",buf[i]);
		}
	}
	printf("\n\n");
}


#if (SPI_MODE_SEL == SPI_MODE_INTERRUPT)
int main(void)
{ 
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0, ENABLE);

	NVIC_Configuration();
	SPI_Configuration();
	SPI_ITConfig(SPIS0, SPI_IT_RXF, ENABLE);
    SPI_ITConfig(SPIS0, SPI_IT_TXE, ENABLE);	
	UART_Configuration();
	printf("MegaHunt SCPU SPI Slave Tx And Rx Demo V1.0.\n");
    
    while (1)
    {
        if (rece_com)
        {
            printf("MegaHunt SCPU SPI Slave Rece Data\n");
            rece_com = 0;
            DataPrintf(data_buf,DATA_BUF_SIZE);
        }
        if (send_com)
        {
            printf("MegaHunt SCPU SPI Slave Send Data\n");
            send_com = 0;
            DataPrintf(data_buf,DATA_BUF_SIZE);
        }
    }
}

#elif (SPI_MODE_SEL == SPI_MODE_POLLING)

int main(void)
{
    uint32_t i = 0;
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0 , ENABLE);

	SPI_Configuration();
	UART_Configuration();
	printf("MegaHunt SCPU SPI Slave Tx And Rx Demo V1.0.\n");

    while(1)
    {
        while (RESET == SPI_GetFlagStatus(SPIS0, SPI_FLAG_RXNE));
        data_buf[i] = SPI_ReceiveData(SPIS0);
        SPI_SendData(SPIS0, data_buf[i]);
        i++;
        
        if (DATA_BUF_SIZE == i)
        {
            DataPrintf(data_buf,DATA_BUF_SIZE);
            i = 0;
        }
    }
}	
#endif

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;
    
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_0);
    
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Null;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_0;
   
    SPI_Init(SPIS0, &SPI_InitStructure);
	SPI_Cmd(SPIS0, ENABLE);
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
