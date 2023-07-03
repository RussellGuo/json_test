#include <stdio.h>
#include <string.h>
#include "mhscpu.h"
#include "systick.h"
#include "ST7789V.h"

extern uint8_t fontAscii[];

LCD_InitTypeDef LCD_InitStructure; 

void LCD_IOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* LCD Remap
	   PC5~PC12:  LCD_DATA0~LCD_DATA7
	   PC13:      LCD_RD
	   PC14:      LCD_WR
	   PC15:      LCD_CD
	 */	
    GPIO_PinRemapConfig(GPIOC, GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | \
                               GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12 | \
                               GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 , GPIO_Remap_0);
    
    /* PD12:       CS
	 */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
    /* PD13:       RESET
	 */	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOD, &GPIO_InitStruct);	

    /* PD14:       LED_EN
	 */	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
}

void LCD_DMAInit(uint8_t *base,uint8_t pageLenth)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//memory to peripheral
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)base;
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = pageLenth;

	DMA_InitStruct.DMA_Peripheral = (uint32_t)LCD;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(LCD->lcdi_data);
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	DMA_Init(DMA_Channel_0, &DMA_InitStruct);
}

void LCD_DMAWriteEnable(uint8_t *buf)
{
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
	DMA_Channel_0->SAR_L = (uint32_t)buf;
	
	DMA_Cmd(ENABLE);
	DMA_ChannelCmd(DMA_Channel_0,ENABLE);
	
	while (!(DMA_GetRawStatus(DMA_Channel_0, DMA_IT_DMATransferComplete)));
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
}

void LCD_WriteEx(uint8_t u8CD, uint8_t u8Value)
{
	LCD_CS_LOW();
	LCD_Write(LCD, u8CD, u8Value);
	LCD_CS_HIGH();
}

void LCD_Reset(void)
{
	LCD_RST_HIGH();
	udelay(100);
	LCD_RST_LOW();
    mdelay(1);
	LCD_RST_HIGH();
    mdelay(1);
}

void LCD_InitSequence(void)
{
    /************* display Setting *************/
    LCD_WriteEx(LCDI_CMD, LCD_CMD_MADCTL);
    LCD_WriteEx(LCDI_DAT, 0xC0);
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_COLMOD);
    LCD_WriteEx(LCDI_DAT, 0x05);     ///< 16bit/pixel
	
	/*********** Frame Rate Setting ***********/
    LCD_WriteEx(LCDI_CMD, LCD_CMD_PORCTRL);
    LCD_WriteEx(LCDI_DAT, 0x0c);     ///< Back porch in normal mode
    LCD_WriteEx(LCDI_DAT, 0x0c);     ///< Front porch in normal mode
    LCD_WriteEx(LCDI_DAT, 0x00);     ///< Disable separate porch control
    LCD_WriteEx(LCDI_DAT, 0x33);     ///< Back and front porch in idle mode
    LCD_WriteEx(LCDI_DAT, 0x33);     ///< Back and front porch in partial mode
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_GCTRL);
    LCD_WriteEx(LCDI_DAT, 0x35);     ///< VGH:13.26, VGL:-10.43
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_VCOMS);
    LCD_WriteEx(LCDI_DAT, 0x35);     ///< VCOM: 1.425
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_LCMCTRL);
    LCD_WriteEx(LCDI_DAT, 0x2c);
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_VDVVRHEN);
    LCD_WriteEx(LCDI_DAT, 0x01);
	LCD_WriteEx(LCDI_DAT, 0xFF);
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_VRHS);    ///< 调整电压参数
    LCD_WriteEx(LCDI_DAT, 0x05);     ///< 3.8+
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_VDVS);
    LCD_WriteEx(LCDI_DAT, 0x20);     ///< VDV: 0V
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_FRCTRL2);
    LCD_WriteEx(LCDI_DAT, 0x0f);     ///< 60Hz
	
    LCD_WriteEx(LCDI_CMD, LCD_CMD_PWCTRL1);
    LCD_WriteEx(LCDI_DAT, 0xa4);
    LCD_WriteEx(LCDI_DAT, 0xa1);     ///< AVDD:6.8V, AVCL:-4.8V, VDDS:2.3V
	
	/************* Gamma Setting **************/
    LCD_WriteEx(LCDI_CMD, LCD_CMD_PVGAMCTRL);
    LCD_WriteEx(LCDI_DAT, 0xd0);
    LCD_WriteEx(LCDI_DAT, 0x00);
    LCD_WriteEx(LCDI_DAT, 0x05);
    LCD_WriteEx(LCDI_DAT, 0x0E);
    LCD_WriteEx(LCDI_DAT, 0x15);
    LCD_WriteEx(LCDI_DAT, 0x0d);
    LCD_WriteEx(LCDI_DAT, 0x37);
    LCD_WriteEx(LCDI_DAT, 0x43);
    LCD_WriteEx(LCDI_DAT, 0x47);
    LCD_WriteEx(LCDI_DAT, 0x09);
    LCD_WriteEx(LCDI_DAT, 0x15);
    LCD_WriteEx(LCDI_DAT, 0x12);
    LCD_WriteEx(LCDI_DAT, 0x16);
    LCD_WriteEx(LCDI_DAT, 0x19);
    
    LCD_WriteEx(LCDI_CMD, LCD_CMD_NVGAMCTRL);
    LCD_WriteEx(LCDI_DAT, 0xd0);
    LCD_WriteEx(LCDI_DAT, 0x00);
    LCD_WriteEx(LCDI_DAT, 0x05);
    LCD_WriteEx(LCDI_DAT, 0x0d);
    LCD_WriteEx(LCDI_DAT, 0x0c);
    LCD_WriteEx(LCDI_DAT, 0x06);
    LCD_WriteEx(LCDI_DAT, 0x2d);
    LCD_WriteEx(LCDI_DAT, 0x44);
    LCD_WriteEx(LCDI_DAT, 0x40);
    LCD_WriteEx(LCDI_DAT, 0x0e);
    LCD_WriteEx(LCDI_DAT, 0x1c);
    LCD_WriteEx(LCDI_DAT, 0x18);
    LCD_WriteEx(LCDI_DAT, 0x16);
    LCD_WriteEx(LCDI_DAT, 0x19);
    
    LCD_WriteEx(LCDI_CMD, LCD_CMD_SLPOUT);     ///< Exit Sleep 
    udelay(120);
    LCD_WriteEx(LCDI_CMD, LCD_CMD_DISPON);     ///< Display on 
}

void LCD_BlockWritePrep(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd)
{
    LCD_WriteEx(LCDI_CMD, LCD_CMD_CASET);
    LCD_WriteEx(LCDI_DAT, xStart >> 8);
    LCD_WriteEx(LCDI_DAT, xStart & 0xff);
    LCD_WriteEx(LCDI_DAT, xEnd >> 8);
    LCD_WriteEx(LCDI_DAT, xEnd & 0xff);
    
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RASET);
    LCD_WriteEx(LCDI_DAT, yStart >> 8);
    LCD_WriteEx(LCDI_DAT, yStart & 0xff);
    LCD_WriteEx(LCDI_DAT, yEnd >> 8);
    LCD_WriteEx(LCDI_DAT, yEnd & 0xff);
    
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RAMWR);
}

void LCD_DisplayColor(uint32_t color)
{
	uint32_t i,j;

	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

	LCD_CS_LOW();
	for (i = 0; i < LCD_ROW_NUM; i++)
	{
	    for (j=0; j < LCD_COL_NUM; j++)
		{    
            LCD_Write(LCD, LCDI_DAT, color >> 8);
            LCD_Write(LCD, LCDI_DAT, color);
		}
	}
	LCD_CS_HIGH();
}

void LCD_DisplayGrayHor16(void)	 
{
	int i, j, k;

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);																								  

	LCD_CS_LOW();
	for (i = 0; i < LCD_ROW_NUM; i++)
	{
		for (j = 0; j < LCD_COL_NUM % 16; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}
		
		for (j = 0; j < 16; j++)
		{
	        for (k = 0;k < LCD_COL_NUM / 16; k++)
			{		               
                LCD_Write(LCD, LCDI_DAT, ((j*2) << 3) | ((j*4) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((j*4) << 5) | (j*2));
			} 
		}
	}
	LCD_CS_HIGH();
}

void LCD_DisplayGrayHor32(void)	 
{
	unsigned int i, j, k;

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);																								  

	LCD_CS_LOW();
	for (i = 0; i < LCD_ROW_NUM; i++)
	{
		for (j = 0; j < LCD_COL_NUM % 32; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}
		
		for (j = 0; j < 32; j++)
		{
	        for (k = 0; k < LCD_COL_NUM / 32; k++)
			{		
                LCD_Write(LCD, LCDI_DAT, (j << 3) | ((j*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((j*2) << 5) | j);
			} 
		}
	}
	LCD_CS_HIGH();	
}

void LCD_DisplayBand(void)	 
{
	int i, j, k;
	unsigned int color[8]={0xf800,0xf800,0x07e0,0x07e0,0x001f,0x001f,0xffff,0xffff};

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

    LCD_CS_LOW();
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < LCD_ROW_NUM / 8; j++)
		{
	        for (k = 0; k < LCD_COL_NUM; k++)
			{
                LCD_Write(LCD, LCDI_DAT, color[i] >> 8);
                LCD_Write(LCD, LCDI_DAT, color[i]);
			} 
		}
	}
    
	for (j = 0; j < LCD_ROW_NUM % 8; j++)
	{
        for (k = 0; k < LCD_COL_NUM; k++)
		{
            LCD_Write(LCD, LCDI_DAT, color[7] >> 8);
            LCD_Write(LCD, LCDI_DAT, color[7]); 
		} 
	}
    LCD_CS_HIGH();
}

void LCD_DisplayFrame(void)
{
    int i;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1); 
    
    LCD_CS_LOW();
	
    LCD_Write(LCD, LCDI_DAT, 0xf8);
    LCD_Write(LCD, LCDI_DAT, 0x00);
    
    for (i = 0; i < LCD_COL_NUM - 2; i++)
    {
        LCD_Write(LCD, LCDI_DAT, 0xf8);
        LCD_Write(LCD, LCDI_DAT, 0x00);
        for (i = 0; i < LCD_COL_NUM - 2; i++)
        {
            LCD_Write(LCD, LCDI_DAT, 0x00);
            LCD_Write(LCD, LCDI_DAT, 0x00);
        }
        LCD_Write(LCD, LCDI_DAT, 0x00);
        LCD_Write(LCD, LCDI_DAT, 0x1F);
    }
    
    LCD_Write(LCD, LCDI_DAT, 0xf8);
    LCD_Write(LCD, LCDI_DAT, 0x00);
    for (i = 0; i < LCD_COL_NUM - 2; i++)
    {
        LCD_Write(LCD, LCDI_DAT, 0xFF);
        LCD_Write(LCD, LCDI_DAT, 0xFF);
    }
    
    LCD_Write(LCD, LCDI_DAT, 0x00);
    LCD_Write(LCD, LCDI_DAT, 0x1F);

    LCD_CS_HIGH();	
}

void LCD_DisplayScaleHor1(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
	
    LCD_CS_LOW();
	/************** black -> red **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)	 
			{
                LCD_Write(LCD, LCDI_DAT, i<<3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
	}

	/************** black -> green **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i*2) >> 3);
                LCD_Write(LCD, LCDI_DAT, (i*2) << 5);
			}
		}
	}

	/************** black -> blue **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, i);
			}
		}
	}

	/************** black -> write **************/
	for (k = 0; k < (LCD_ROW_NUM / 4 + LCD_ROW_NUM % 4); k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i << 3) | ((i*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((i*2) << 5) | i);
			}
		}
	}
	
	LCD_CS_HIGH();	
}

void LCD_DisplayScaleHor2(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
	
    LCD_CS_LOW();
	/************** black -> red **************/		
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {                
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM/32; j++)	 
			{
                LCD_Write(LCD, LCDI_DAT, i << 3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
	}

	/************** red -> black **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{	
		for (i = 32; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM/32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i-1) << 3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}
	
	/************** black -> green **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 64; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 64; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 64; j++)
			{
                LCD_Write(LCD, LCDI_DAT, i >> 3);
                LCD_Write(LCD, LCDI_DAT, i << 5);
			}
		}
	}

	/************** green -> black **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 64; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM/64; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i-1) >> 3);
                LCD_Write(LCD, LCDI_DAT, (i-1) << 5);
			}
		}
		for (i = 0; i < LCD_COL_NUM % 64; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}	

	/************** black -> blue **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, i);
			}
		}
	}

	/************** blue -> black **************/
	for (k = 0; k < (LCD_ROW_NUM / 6 + LCD_ROW_NUM % 6); k++)
	{	
		for (i = 32; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, (i-1));  
			}
		}
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}
	
	LCD_CS_HIGH();		
}

void LCD_DisplayScaleVer(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

	LCD_CS_LOW();
	/*********** LCD_DISP_BLACK ***********/
	for (k = 0; k < LCD_ROW_NUM % 32; k++)
	{
		for (j = 0; j < LCD_COL_NUM; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}			
	}

	for (k = 0; k < 32; k++)
	{
		for (i = 0; i < LCD_ROW_NUM / 32; i++)
		{	
			/******** LCD_DISP_RED ********/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, k<<3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
			/******* LCD_DISP_GREEN *******/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (k*2) >> 3);
                LCD_Write(LCD, LCDI_DAT, (k*2) << 5);
			}
			/******* LCD_DISP_BLUE *******/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, k);
			}
			/******* LCD_DISP_GRAY *******/										  
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (k << 3) | ((k*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((k*2) << 5) | k);
			}
		}
	}
	
	LCD_CS_HIGH();	
}

void LCD_DisplayPicDirect(const uint8_t *picture)
{
    uint32_t i;
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
    
	LCD_CS_LOW();
    for (i = 0; i < LCD_COL_NUM*LCD_ROW_NUM*2; i++)
	{
        LCD_Write(LCD, LCDI_DAT, (*(picture + i)));
    }
	LCD_CS_HIGH();		
}

void LCD_DisplayPicDMA(uint8_t *picture)
{
    uint32_t i;
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
    
    LCD_DMAInit(picture, (uint8_t)(LCD_COL_NUM)); 
    LCD_FIFODMAconfig(LCD, &LCD_InitStructure);
	
	LCD_CS_LOW();	
    for (i = 0; i < LCD_ROW_NUM*2; i++)
    {
        LCD->lcdi_ctrl |= LCDI_CTRL_CD;
        LCD_DMAWriteEnable(picture + i*LCD_COL_NUM);
    }
  	LCD_CS_HIGH();  
}

void LCD_WriteOneDot(uint32_t color)
{
	LCD_CS_LOW();
	
	LCD_Write(LCD, LCDI_DAT, color >> 8);
	LCD_Write(LCD, LCDI_DAT, color);
	
  	LCD_CS_HIGH(); 
}

void LCD_DisplayOneChar(uint8_t ord, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor)	///< ord: 0~95
{													  
	uint8_t i, j;
	uint8_t *p, data;
	uint32_t index;

	LCD_BlockWritePrep(xStart, xStart + (LCD_FONT_W - 1), yStart, yStart+(LCD_FONT_H - 1));

	index = ord;
	if (index > 95)	   //95:ASCII CHAR NUM
	{
		index = 95; 
	}

	index = index * ((LCD_FONT_W/8) * LCD_FONT_H);	 

	p = fontAscii + index;
	for (i = 0; i < (LCD_FONT_W/8*LCD_FONT_H); i++)
	{
		data = *p++;
		for (j = 0; j < 8; j++)
		{
			if ((data << j) & 0x80)
			{
				LCD_WriteOneDot(textColor);
			}      
			else 
			{
				LCD_WriteOneDot(backColor);	  
			}
		}
	}
}

/*
 * ascii 32~90(!~Z), (32~47)空格~/,(48~57)0~9,(58~64):~@,(65~126)A~~
 * ord 0~95, (48~57)0~9,(65~126)A~z,(33~47)!~/,(58~64):~@
 */
static uint8_t s_toOrd(uint8_t ch)
{
	if (ch < 32)
	{
		ch = 95;
	}
	else if ((ch >= 32) && (ch <= 47))  ///< (32~47)空格~/
	{
		ch = (ch - 32) + 10 + 62;
	}
	else if ((ch >= 48) && (ch <= 57))  ///< (48~57)0~9
	{
		ch = ch - 48;
	}
	else if ((ch >= 58) && (ch <= 64))  ///< (58~64):~@
	{
		ch = (ch - 58) + 10 + 62 + 16;
	}
	else if ((ch >= 65) && (ch <= 126)) ///< (65~126)A~~
	{
		ch = (ch - 65) + 10;
	}
	else if (ch > 126)
	{		
		ch = 95;
	}

	return ch;
}

void LCD_DisplayStr(uint8_t *str, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor)
{
	while (!(*str == '\0'))
	{
		LCD_DisplayOneChar(s_toOrd(*str++), xStart, yStart, textColor, backColor);

		if (xStart > ((LCD_COL_NUM-1) - LCD_FONT_W))
		{
			xStart = 0;
		    yStart = yStart + LCD_FONT_H;
		}
		else
		{
			xStart = xStart + LCD_FONT_W;
		}

		if (yStart > ((LCD_ROW_NUM - 1) - LCD_FONT_H))
		{
			yStart=0;
		}
	}
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
}

void LCD_DrawLine(uint32_t xStart, uint32_t xEnd, uint32_t yStart, uint32_t yEnd, uint32_t color)
{
	uint32_t i, j;

	LCD_BlockWritePrep(xStart, xEnd, yStart, yEnd);

	for (i = yStart; i < yEnd + 1; i++)
	{
		for (j = xStart; j < xEnd + 1; j++)
		{
			LCD_WriteOneDot(color);
		}
	}
}

void LCD_DrawGird(uint32_t color)
{
	uint32_t i;

	for (i = 15; i < LCD_ROW_NUM-1; i = i+16)
	{
		LCD_DrawLine(0, LCD_COL_NUM-1, i, i, color);
	}

	for (i = 15; i < LCD_COL_NUM-1; i = i+16)
	{
		LCD_DrawLine(i, i, 0, LCD_ROW_NUM-1, color);
	}
}
