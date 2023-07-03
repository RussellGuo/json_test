#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "lcd.h"
#include "timer.h"

static void TFT_LCD_WriteData8(uint8_t data);
static void TFT_LCD_WriteData(uint16_t data);
static void TFT_LCD_WriteReg(uint8_t data);
static void TFT_LCD_SetAddress(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
static uint32_t pow(uint8_t m, uint8_t n);
extern HSPIM_TypeDef *HSPIx;

void TFT_LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	HSPIM_InitTypeDef HSPIM_InitStructure;
 	HSPIM_DMAInitTypeDef HSPIM_DMAInitStructure;
    
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_13 | GPIO_Pin_15, GPIO_Remap_3);  
    
    /* CS */
    GPIO_InitStructure.GPIO_Pin = TFT_LCD_CS_PIN_NUM;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
	
    GPIO_Init(TFT_LCD_CS_PIN_GROUP, &GPIO_InitStructure);
    TFT_LCD_CS_HIGH();
    
    /* RESET */
    GPIO_InitStructure.GPIO_Pin = TFT_LCD_RST_PIN_NUM;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
   
    GPIO_Init(TFT_LCD_RST_PIN_GROUP, &GPIO_InitStructure);
    TFT_LCD_RST_HIGH(); 

    /* DC */
    GPIO_InitStructure.GPIO_Pin = TFT_LCD_DC_PIN_NUM;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
   
    GPIO_Init(TFT_LCD_DC_PIN_GROUP, &GPIO_InitStructure);
    TFT_LCD_DC_HIGH();
	
	HSPIM_DMAInitStructure.HSPIM_DMATransmitEnable = ENABLE;
	HSPIM_DMAInitStructure.HSPIM_DMATransmitLevel = HSPIM_DMA_TRANSMIT_LEVEL_31;	
	HSPIM_DMAInit(HSPIx,&HSPIM_DMAInitStructure);	

    /* SPI */

	HSPIM_InitStructure.HSPIM_Mode = HSPIM_MODE_STANDARD;
	HSPIM_InitStructure.HSPIM_CPOL = HSPIM_CPOL_HIGH;
	HSPIM_InitStructure.HSPIM_CPHA = HSPIM_CPHA_2EDGE;
	HSPIM_InitStructure.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	HSPIM_InitStructure.HSPIM_DivideEnable = ENABLE;
	HSPIM_InitStructure.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_2;
	HSPIM_InitStructure.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_8;
	HSPIM_InitStructure.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_8;	
	
	HSPIM_FIFOReset(HSPIx,HSPIM_FIFO_TX | HSPIM_FIFO_RX);
	HSPIM_Init(HSPIx,&HSPIM_InitStructure);
	HSPIM_Cmd(HSPIx, ENABLE);
	HSPIM_TransmitCmd(HSPIx,ENABLE);

    /* RESET */
    TFT_LCD_CS_LOW();
	TFT_LCD_RST_LOW();
	Delay(20);
	TFT_LCD_RST_HIGH();
	Delay(20);

    /* TFT LCD Init */
    TFT_LCD_WriteReg(TFT_LCD_CMD_PWCTRL1);
    TFT_LCD_WriteData8(0x21);    ///< VRH[5:0]
    
    TFT_LCD_WriteReg(TFT_LCD_CMD_PWCTRL2);
    TFT_LCD_WriteData8(0x12);    ///< BT[2:0]
    
    TFT_LCD_WriteReg(TFT_LCD_CMD_VCOMCTRL1);
    TFT_LCD_WriteData8(0x32);    ///< VMH[6:0]
    TFT_LCD_WriteData8(0x3C);    ///< VML[6:0]

    TFT_LCD_WriteReg(TFT_LCD_CMD_VCOMCTRL2);
    TFT_LCD_WriteData8(0xC1);    ///< VMF[6:0]

    TFT_LCD_WriteReg(TFT_LCD_CMD_MEMACCESSCTRL);
    TFT_LCD_WriteData8(0x08);

    TFT_LCD_WriteReg(TFT_LCD_CMD_COLMOD);
    TFT_LCD_WriteData8(0x55);
    
    TFT_LCD_WriteReg(TFT_LCD_CMD_FRAMERATECTRL);
    TFT_LCD_WriteData8(0x00);    ///< DIVA[1:0]
    TFT_LCD_WriteData8(0x18);    ///< RTNA[4:0]
    
    TFT_LCD_WriteReg(TFT_LCD_CMD_DISFUNCCTRL);
    TFT_LCD_WriteData8(0x0A);
    TFT_LCD_WriteData8(0xA2);

    TFT_LCD_WriteReg(TFT_LCD_CMD_GAMMASET);
    TFT_LCD_WriteData8(0x01);    ///< GC[7:0]
    
    TFT_LCD_WriteReg(TFT_LCD_CMD_PGAMCTR);
    TFT_LCD_WriteData8(0x0F);
    TFT_LCD_WriteData8(0x20); 
    TFT_LCD_WriteData8(0x1E); 
    TFT_LCD_WriteData8(0x09); 
    TFT_LCD_WriteData8(0x12); 
    TFT_LCD_WriteData8(0x0B); 
    TFT_LCD_WriteData8(0x50); 
    TFT_LCD_WriteData8(0XBA); 
    TFT_LCD_WriteData8(0x44); 
    TFT_LCD_WriteData8(0x09); 
    TFT_LCD_WriteData8(0x14); 
    TFT_LCD_WriteData8(0x05); 
    TFT_LCD_WriteData8(0x23); 
    TFT_LCD_WriteData8(0x21); 
    TFT_LCD_WriteData8(0x00); 

    TFT_LCD_WriteReg(TFT_LCD_CMD_NGAMCTRL);
    TFT_LCD_WriteData8(0x00); 
    TFT_LCD_WriteData8(0x19); 
    TFT_LCD_WriteData8(0x19); 
    TFT_LCD_WriteData8(0x00); 
    TFT_LCD_WriteData8(0x12); 
    TFT_LCD_WriteData8(0x07); 
    TFT_LCD_WriteData8(0x2D); 
    TFT_LCD_WriteData8(0x28); 
    TFT_LCD_WriteData8(0x3F); 
    TFT_LCD_WriteData8(0x02); 
    TFT_LCD_WriteData8(0x0A); 
    TFT_LCD_WriteData8(0x08); 
    TFT_LCD_WriteData8(0x25); 
    TFT_LCD_WriteData8(0x2D); 
    TFT_LCD_WriteData8(0x0F);

    TFT_LCD_WriteReg(TFT_LCD_CMD_SLPOUT);
    Delay(120);
    TFT_LCD_WriteReg(TFT_LCD_CMD_DISPON);
}

void TFT_LCD_Clear(uint16_t Color)
{
	uint16_t i, j; 
 	
	TFT_LCD_SetAddress(0, 0, TFT_LCD_WIDTH-1, TFT_LCD_HIGHT-1);
    
    for (i = 0; i < TFT_LCD_WIDTH; i++)
    {
        for (j = 0; j < TFT_LCD_HIGHT; j++)
        {
           TFT_LCD_WriteData(Color);	 			 
        }
    }    
}

static void TFT_LCD_SetAddress(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
	TFT_LCD_WriteReg(TFT_LCD_CMD_CASET);
    TFT_LCD_WriteData8(x1 >> 8);
    TFT_LCD_WriteData8(x1);
    TFT_LCD_WriteData8(x2 >> 8);
    TFT_LCD_WriteData8(x2);
  
    TFT_LCD_WriteReg(TFT_LCD_CMD_RASET);
    TFT_LCD_WriteData8(y1 >> 8);
    TFT_LCD_WriteData8(y1);
    TFT_LCD_WriteData8(y2 >> 8);
    TFT_LCD_WriteData8(y2);

    TFT_LCD_WriteReg(0x2C);    
}

static void TFT_LCD_WriteData8(uint8_t data)
{
    TFT_LCD_CS_LOW();
    TFT_LCD_DC_HIGH();
    
	HSPIM_SendData(HSPIx,data);
	while (RESET == HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_TXE));
    
    TFT_LCD_CS_HIGH();    
}

static void TFT_LCD_WriteData(uint16_t data)
{
    TFT_LCD_CS_LOW();
    TFT_LCD_DC_HIGH();
    
    HSPIM_SendData(HSPIx,((data >> 8) & 0xFF));
	while (RESET == HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_TXE));

    HSPIM_SendData(HSPIx,data & 0xFF);
	while (RESET == HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_TXE));
    
    TFT_LCD_CS_HIGH(); 
}

static void TFT_LCD_WriteReg(uint8_t data)
{
    TFT_LCD_CS_LOW();
    TFT_LCD_DC_LOW();
    
    HSPIM_SendData(HSPIx,data);
	while (RESET == HSPIM_GetFlagStatus(HSPIx,HSPIM_FLAG_TXE));
    
    TFT_LCD_CS_HIGH();    
}

void TFT_LCD_DrawPoint(POINT *point, uint16_t color)
{
	TFT_LCD_SetAddress(point->x, point->y, point->x, point->y);//设置光标位置 
	TFT_LCD_WriteData(color); 	
}

void TFT_LCD_DrawPointBig(uint16_t x, uint16_t y, uint16_t color)
{
    RECT rect;

    rect.left = x-1;
    rect.top = y-1;
    rect.right = x+1;
    rect.bottom = y+1;

    TFT_LCD_Fill(&rect, color);
}

void TFT_LCD_DrawLine(POINT *startPoint, POINT *endPoint, uint16_t color)
{
	uint16_t t; 
	int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
	int incx, incy, uRow, uCol; 
    POINT point;

	delta_x = endPoint->x - startPoint->x;   ///< 计算坐标增量 
	delta_y = endPoint->y - startPoint->y; 
	uRow = startPoint->x; 
	uCol = startPoint->y;
    
	if (delta_x > 0)
    {
        incx = 1;    ///< 设置单步方向 
    }
	else if (delta_x == 0)
    {
        incx = 0;    ///< 垂直线 
    }
	else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
	if (delta_y > 0)
    {
        incy = 1; 
    }
	else if (delta_y == 0)
    {
        incy = 0;   ///< 水平线 
    }
	else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
	if (delta_x > delta_y)
    {
        distance = delta_x;    ///< 选取基本增量坐标轴 
    }
	else
    {
        distance = delta_y; 
    }
    
	for (t = 0; t <= distance+1; t++ )     ///< 画线输出 
	{
        point.x = uRow;
        point.y = uCol;
		TFT_LCD_DrawPoint(&point, color);  ///< 画点 
		xerr += delta_x; 
		yerr += delta_y;
        
		if (xerr > distance) 
		{
			xerr -= distance; 
			uRow += incx; 
		} 
		if (yerr > distance) 
		{ 
			yerr -= distance; 
			uCol += incy; 
		} 
	}  
}

void TFT_LCD_DrawRectangle(RECT *rect, uint16_t color)
{
    POINT startPoint, endPoint;
    
    startPoint.x = rect->left;
    startPoint.y = rect->top;
    endPoint.x = rect->right;
    endPoint.y = rect->top;
	TFT_LCD_DrawLine(&startPoint, &endPoint, color);
    
    startPoint.x = rect->left;
    startPoint.y = rect->top;
    endPoint.x = rect->left;
    endPoint.y = rect->bottom;    
	TFT_LCD_DrawLine(&startPoint, &endPoint, color);

    startPoint.x = rect->left;
    startPoint.y = rect->bottom;
    endPoint.x = rect->right;
    endPoint.y = rect->bottom;        
	TFT_LCD_DrawLine(&startPoint, &endPoint, color);
    
    startPoint.x = rect->right;
    startPoint.y = rect->top;
    endPoint.x = rect->right;
    endPoint.y = rect->bottom;      
	TFT_LCD_DrawLine(&startPoint, &endPoint, color);
}

void TFT_LCD_Fill(RECT *rect, uint16_t color)
{
	uint16_t x, y; 
    
	TFT_LCD_SetAddress(rect->left, rect->top, rect->right, rect->bottom);      ///< 设置光标位置 
	
    for (y = rect->top; y <= rect->bottom; y++)
	{													   	 	
		for (x = rect->left; x <= rect->right; x++)
        {
            TFT_LCD_WriteData(color);  
        }
	}
}

void TFT_LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t mode, uint16_t backColor, uint16_t pointColor)
{
    uint8_t temp;
    uint8_t pos, t;
	uint16_t x0 = x;
    uint16_t color;
    POINT point;
    
    if (x > TFT_LCD_WIDTH-16 || y > TFT_LCD_HIGHT-16)
    {
        return;
    }
    
	///< 设置窗口		   
	num = num - ' ';   ///< 得到偏移后的值
	TFT_LCD_SetAddress(x, y, x+8-1, y+16-1);                 ///< 设置光标位置 
	if (!mode) //非叠加方式
	{
		for (pos = 0; pos < 16; pos++)
		{ 
			temp = ascii_1608[(uint16_t)num*16 + pos];		 ///< 调用1608字体
			for (t = 0; t < 8; t++)
		    {                 
		        if (temp & 0x01)
                {
                    color = pointColor;
                }
				else
                {
                    color = backColor;
                }
				TFT_LCD_WriteData(color);	
				temp >>= 1; 
				x++;
		    }
			x = x0;
			y++;
		}	
	}
    else     ///< 叠加方式
	{
		for (pos = 0; pos < 16; pos++)
		{
		    temp = ascii_1608[(uint16_t)num*16 + pos];		 ///< 调用1608字体
			for (t = 0; t < 8; t++)
		    {                 
		        if (temp & 0x01)
                {
                    point.x = x + t;
                    point.y = y + pos;
                    TFT_LCD_DrawPoint(&point, pointColor);   ///< 画一个点  
                }   
		        temp >>= 1; 
		    }
		}
	}
}

void TFT_LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint16_t backColor, uint16_t pointColor)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	num = (uint16_t)num;
    
	for (t = 0; t < len; t++)
	{
		temp = (num / pow(10, len-t-1)) % 10;
		if (enshow == 0 && t < (len-1))
		{
			if (temp == 0)
			{
				TFT_LCD_ShowChar(x + 8*t, y, ' ', 0, backColor, pointColor);
				continue;
			}
            else
            {
                enshow = 1; 
            }
		}
	 	TFT_LCD_ShowChar(x + 8*t, y, temp+48, 0, backColor, pointColor); 
	}
}

void TFT_LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t backColor, uint16_t pointColor)
{
    while (*p != '\0')
    {       
        if (x > TFT_LCD_WIDTH - 16)
        {
            x = 0;
            y += 16;
        }
        
        if (y > TFT_LCD_HIGHT - 16)
        {
            y = x = 0;
            TFT_LCD_Clear(TFT_LCD_COLOR_RED);
        }
        TFT_LCD_ShowChar(x, y, *p, 0, backColor, pointColor);
        x += 8;
        p++;
    }  
}

///< m^n
static uint32_t pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
    
	while (n--)
    {
        result *= m;
    }
    
	return result;
}


extern uint8_t gImage[153600];

void TFT_LCD_ShowImageDirect(void)
{
    int i, j;
    uint8_t HighData, LowData;
	uint8_t gBuffer[TFT_LCD_WIDTH*TFT_LCD_REFRESH_LINES*2] = {0};

	TFT_LCD_Clear(TFT_LCD_COLOR_WHITE);

	for (i = 0; i < TFT_LCD_HIGHT/TFT_LCD_REFRESH_LINES; i++)
	{
		TFT_LCD_SetAddress(0, i*TFT_LCD_REFRESH_LINES, TFT_LCD_WIDTH-1, (i+1)*TFT_LCD_REFRESH_LINES-1);	
		
		memcpy(gBuffer, (uint8_t *)(gImage+i*TFT_LCD_WIDTH*TFT_LCD_REFRESH_LINES*2), sizeof(gBuffer));
		for (j = 0; j < TFT_LCD_WIDTH*TFT_LCD_REFRESH_LINES; j++)
		{
			HighData = *(gBuffer + j*2);  ///< MSB First
			LowData = *(gBuffer + j*2 + 1);
			
			TFT_LCD_WriteData((HighData << 8) | LowData);
		}			
	}	
}


void TFT_LCD_ShowImageDMA(void)
{
    uint32_t i;
 	DMA_InitTypeDef dma;
    uint8_t buffer[TFT_LCD_DMA_BLOCK_SIZE] = {0};

	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE); 
    
	dma.DMA_DIR = DMA_DIR_Memory_To_Peripheral;	
	dma.DMA_Peripheral = (uint32_t)(HSPIx);
	dma.DMA_PeripheralBaseAddr = (uint32_t)&HSPIx->WDR;
	dma.DMA_PeripheralInc = DMA_Inc_Nochange;
	dma.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	dma.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	dma.DMA_MemoryBaseAddr = (uint32_t)buffer;
	dma.DMA_MemoryInc = DMA_Inc_Increment;
	dma.DMA_MemoryDataSize = DMA_DataSize_Byte;
	dma.DMA_MemoryBurstSize = DMA_BurstSize_1;
	dma.DMA_BlockSize = TFT_LCD_DMA_BLOCK_SIZE;
	dma.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	
    DMA_Init(DMA_Channel_0, &dma); 
	DMA_Cmd(ENABLE);

	TFT_LCD_Clear(TFT_LCD_COLOR_WHITE); 
 	TFT_LCD_SetAddress(0, 0, TFT_LCD_WIDTH-1, TFT_LCD_HIGHT-1);	
	
    TFT_LCD_CS_LOW();
    TFT_LCD_DC_HIGH();
    
	for (i = 0; i < (TFT_LCD_WIDTH*TFT_LCD_HIGHT*2 / TFT_LCD_DMA_BLOCK_SIZE); i++)
	{
		memcpy(buffer, (uint8_t *)(gImage+i*TFT_LCD_DMA_BLOCK_SIZE), TFT_LCD_DMA_BLOCK_SIZE);
		
        DMA_SetSRCAddress(DMA_Channel_0, (uint32_t)buffer);
		DMA_ChannelCmd(DMA_Channel_0, ENABLE);    
        while (DMA_IsChannelEnabled(DMA_Channel_0) != DISABLE);  
	}

    TFT_LCD_CS_HIGH();     
}
