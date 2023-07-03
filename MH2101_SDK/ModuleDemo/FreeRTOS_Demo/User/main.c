#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "FreeRTOS.h"
#include "task.h"

#define BEEP_GPIO                  GPIOC
#define BEEP_GOIO_PIN              GPIO_Pin_9

void UART_Configuration(void);
void Beep_Task(void const * arg);
void CRC_Task(void const * arg);
void Delay_Ms(uint32_t num);
void BEEP_Init(void);
void beep(int ms);

int main(void)
{	
	SYSCTRL_PLLConfig(SYSCTRL_PLL_168MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_CRC, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_CRC ,ENABLE);
	
	UART_Configuration();
	printf("MH1903S freeRTOS demo.\n");
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	BEEP_Init();
	beep(100);
	/* System interrupt init*/
	NVIC_SetPriority(MemoryManagement_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_3, 0, 0));
	NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 0, 0));
	NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 0, 0));
	NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 0, 0));
	NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 0, 0));
	NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 7, 0));
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_3, 7, 0));
	
	xTaskCreate((TaskFunction_t )Beep_Task,
                (const char*    )"Beep_Task",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )0,
                (TaskHandle_t*  )NULL);
				
	xTaskCreate((TaskFunction_t )CRC_Task,
                (const char*    )"CRC_Task",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )NULL);  
				 		
	vTaskStartScheduler();
	while (1);

}
void Beep_Task(void const * arg)
{
	BEEP_Init();
	for (;;)
	{
		beep(600);
		vTaskDelay(2000);
	}
}

void CRC_Task(void const * arg)
{
	uint8_t str[] = "123456789";
	for (;;)
	{	
		printf("CRC_16 :            		%08X \r\n",CRC_CalcBlockCRC(CRC_16,str,sizeof(str)-1));	
		vTaskDelay(2000);
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

void Delay_Ms(uint32_t num)
{
    int i;
    
	for(i = 0; i<num*10000; i++);
}

void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = BEEP_GOIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStruct);
	GPIO_SetBits(BEEP_GPIO,BEEP_GOIO_PIN);
}
void beep(int ms)
{	
	GPIO_ResetBits(BEEP_GPIO,BEEP_GOIO_PIN);
	Delay_Ms(ms);
	GPIO_SetBits(BEEP_GPIO,BEEP_GOIO_PIN);
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
