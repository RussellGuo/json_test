/************************ (C) COPYRIGHT Megahuntmicro ********************************************************
 * @file                : main.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 9-November-2019
 *************************************************************************************************************/
#include <stdio.h>
#include "mhscpu.h"
#include "lcd_init.h"
#include "lcd.h"
#include "delay.h"
#include "touch.h"

void UART_Configuration(void);



int main(void)
{
	uint32_t ret;
	Point_t tp;
	
	SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
	
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);

	UART_Configuration();
	printf(">>>>> Megahunt TP demo v1.0\n");

    SYSTICK_Init();
    
	LCD_Init();//LCD≥ı ºªØ
	TftLCD_ClearDma(WHITE);  

   	TftLCD_ShowString(10, 30, "Megahunt Micro Touch Demo", RED, WHITE, 16, 0); 
    
    mdelay(2000);
	TftLCD_ClearDma(WHITE); 
    
    TP_AdcInit();   
    
	while(TP_Calibrate() == 0);
    
    TftLCD_ShowString(0, 0, "RESET", RED, WHITE, 16, 0); 
    
    while(1)
    {
        if(TP_Scan(&tp)==0)
        {
            if(tp.x < 38 && tp.y < 15)
            {
				TftLCD_ClearDma(WHITE); 
                TftLCD_ShowString(0, 0, "RESET", RED, WHITE, 16, 0);
            }
            else
            {
                TP_DrawBigPoint(tp.x,tp.y, RED);
            }
            
            //printf("Display:%d,%d.\n", tp.x, tp.y);
        }
        else
        {
            mdelay(10);
        }
    }
}


void UART_Configuration(void)
{
	UART_InitTypeDef uart;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	uart.UART_BaudRate = 115200;
	uart.UART_WordLength = UART_WordLength_8b;
	uart.UART_StopBits = UART_StopBits_1;
	uart.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &uart);
}


int SER_PutChar(int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);

	return ch;
}

int fputc(int c, FILE *f)
{
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while(1);
}
#endif

