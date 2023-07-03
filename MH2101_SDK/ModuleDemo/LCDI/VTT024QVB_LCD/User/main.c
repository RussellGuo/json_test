#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "systick.h"
#include "ST7789V.h"

#define DMA_FLUSH_SCREEN	       (1)
//#define DIRECT_FLUSH_SCREEN	       (1)

extern uint8_t imageTest1[];
extern uint8_t imageTest2[];

void UART_Configuration(void);
void LCD_Test(void);

int main(void)
{
    SYSCTRL_PLLConfig(SYSCTRL_PLL_168MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_DMA, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_DMA, ENABLE); 
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

	SYSTICK_Init();	
    LCD_IOConfig();
	UART_Configuration();

	printf("MegaHunt SCPU VTT024QVB LCD Demo V1.0.\n");

    LCD_Reset();
    
    LCD_InitStructure.LCD_BusMode = LCDI_MODE_8080;
    LCD_InitStructure.LCD_IntRead = 2;
    LCD_InitStructure.LCD_IntWrite = 0;
#ifdef DMA_FLUSH_SCREEN		
    LCD_InitStructure.LCD_MaxQTR = 0;
#else
    LCD_InitStructure.LCD_MaxQTR = 2;
#endif
    LCD_InitStructure.fifo_level = 16;   
    LCD_InitStructure.fifo_rst_enable = ENABLE;
    LCD_InitStructure.wr_dma_enable = ENABLE;
    LCD_InitStructure.wr_fifo_enable = ENABLE;

#if DIRECT_FLUSH_SCREEN	  
    LCD_InitStructure.ring_buf.rbRead.u32Head = 0;
    LCD_InitStructure.ring_buf.rbRead.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbRead.u32BuffSize = LCD_READ_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbRead.pu8Buff = LCD_InitStructure.ring_buf.lcd_ReadBuff;

    LCD_InitStructure.ring_buf.rbWrite.u32Head = 0;
    LCD_InitStructure.ring_buf.rbWrite.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbWrite.u32BuffSize = LCD_WRITE_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbWrite.pu8Buff = LCD_InitStructure.ring_buf.lcd_WriteBuff;

    LCD_InitStructure.ring_buf.rbCmd.u32Head = 0;
    LCD_InitStructure.ring_buf.rbCmd.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbCmd.u32BuffSize = LCD_CMD_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbCmd.pu32Buff = LCD_InitStructure.ring_buf.lcd_CmdBuff;
    LCD_InitStructure.ring_buf.u32OpCount = 0;

    LCD_InitStructure.ring_buf.prbCmd = &LCD_InitStructure.ring_buf.rbCmd;
    LCD_InitStructure.ring_buf.prbRead = &LCD_InitStructure.ring_buf.rbRead;
    LCD_InitStructure.ring_buf.prbWrite = &LCD_InitStructure.ring_buf.rbWrite; 
#endif
    
    LCD_Init(LCD, &LCD_InitStructure);
    
    LCD_Test();
	
	while(1);
}


void LCD_Test(void)
{
    LCD_InitSequence();
    
    /************* 颜色测试 *************/
	printf(">>>>>>  Color Test\n");
    LCD_DisplayColor(LCD_DISP_WRITE);
    mdelay(200);
    LCD_DisplayColor(LCD_DISP_RED);
    mdelay(200);
    LCD_DisplayColor(LCD_DISP_GREEN);
    mdelay(200);
    LCD_DisplayColor(LCD_DISP_BLUE);
    mdelay(2000);

    /************* 灰度测试 *************/
	printf(">>>>>>  Gray Test\n");	
    LCD_DisplayGrayHor16();    ///< 240/16密度
    mdelay(200);
    LCD_DisplayGrayHor32();    ///< 240/32密度
    mdelay(2000);
    
    /***** 一屏多色，并且逐渐变换 ******/
	printf(">>>>>>  Multi Color Test\n");		
    LCD_DisplayScaleHor1();    //纵轴方向变换
    mdelay(200);
    LCD_DisplayScaleHor2();
    mdelay(200);
    LCD_DisplayScaleVer();     //横轴方向变化    
    mdelay(2000);

    /************ 文字测试 ************/
	printf(">>>>>>  Text Test\n");
	LCD_DisplayColor(LCD_DISP_WRITE);
	LCD_DisplayStr("MEGAHUNT MICRO", 60, 150, LCD_DISP_BLUE, LCD_DISP_WRITE);
    mdelay(2000);

    /************ 图形测试 ************/
	printf(">>>>>>  Graph Test\n");
	LCD_DisplayColor(LCD_DISP_WRITE);	
	LCD_DrawLine(0, 240, 50, 60, LCD_DISP_RED);
	LCD_DrawLine(0, 200, 70, 80, LCD_DISP_GREEN);
	LCD_DrawLine(0, 160, 90, 100, LCD_DISP_GRAY);
	LCD_DrawLine(0, 120, 110, 120, LCD_DISP_GRAY50);
    mdelay(2000);
	LCD_DisplayColor(LCD_DISP_WRITE);
	LCD_DrawGird(LCD_DISP_RED);
    mdelay(2000);
	
	printf(">>>>>>  Picture Test\n");
#if DIRECT_FLUSH_SCREEN        
    while (1)
    {
        mdelay(500);
        LCD_DisplayPicDirect(imageTest1);  //图片大小为320x240
        mdelay(500);
        LCD_DisplayPicDirect(imageTest2);
        mdelay(500);
    }
#endif

#if DMA_FLUSH_SCREEN
    while (1)
    {
        LCD_DisplayPicDMA(imageTest1);     //图片大小为320x240
        mdelay(500);
        LCD_DisplayPicDMA(imageTest2);
        mdelay(500);
    }
#endif
}


void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0|GPIO_Pin_1, GPIO_Remap_0);	
	
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
