#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"
#include "delay.h"


/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
								color       要填充的颜色
      返回值：  无
******************************************************************************/
void TftLCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
	
    LCD_Address_Set(xsta, ysta, xend-1, yend-1);//设置显示范围
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

void TftLCD_ClearDma(uint16_t color)
{
    uint32_t i;
 	DMA_InitTypeDef DMA_InitStruct;
    uint8_t buffer[TFT_LCD_DMA_BLOCK_SIZE] = {0};

	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE); 
    
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;	
	DMA_InitStruct.DMA_Peripheral = (uint32_t)(LCD_SPI);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&LCD_SPI->WDR;
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buffer;
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_4;
	DMA_InitStruct.DMA_BlockSize = TFT_LCD_DMA_BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	
    DMA_Init(DMA_Channel_0, &DMA_InitStruct); 
	DMA_Cmd(ENABLE);

	memset(buffer, color, TFT_LCD_DMA_BLOCK_SIZE);	
	
 	LCD_Address_Set(0, 0, TFTLCD_WIDTH-1, TFTLCD_HEIGHT-1);	
	
    LCD_CS_Clr();
    
	for (i = 0; i < (TFTLCD_WIDTH*TFTLCD_HEIGHT*2 / TFT_LCD_DMA_BLOCK_SIZE); i++)
	{
        DMA_SetSRCAddress(DMA_Channel_0, (uint32_t)buffer);
		DMA_ChannelCmd(DMA_Channel_0, ENABLE);    
		while (RESET == DMA_GetRawStatus(DMA_Channel_0, DMA_IT_DMATransferComplete));
		DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
	}

	udelay(10);
    LCD_CS_Set();     
}


/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void TftLCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y);//设置光标位置
    LCD_WR_DATA(color);
}

/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void TftLCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
	
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if (delta_x==0)incx=0;//垂直线
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线
    else {
        incy=-1;
        delta_y=-delta_y;
    }
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<distance+1; t++)
    {
        TftLCD_DrawPoint(uRow,uCol,color);//画点
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    TftLCD_DrawLine(x1,y1,x2,y1,color);
    TftLCD_DrawLine(x1,y1,x1,y2,color);
    TftLCD_DrawLine(x1,y2,x2,y2,color);
    TftLCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void TftLCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a, b;
	
    a = 0;
    b = r;
    while (a <= b)
    {
        TftLCD_DrawPoint(x0-b,y0-a,color);             //3
        TftLCD_DrawPoint(x0+b,y0-a,color);             //0
        TftLCD_DrawPoint(x0-a,y0+b,color);             //1
        TftLCD_DrawPoint(x0-a,y0-b,color);             //2
        TftLCD_DrawPoint(x0+b,y0+a,color);             //4
        TftLCD_DrawPoint(x0+a,y0-b,color);             //5
        TftLCD_DrawPoint(x0+a,y0+b,color);             //6
        TftLCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}

/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void TftLCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
    uint8_t temp,sizex,t;
    uint16_t i,TypefaceNum;//一个字符所占字节大小
    uint16_t x0=x;
    sizex=sizey/2;
    TypefaceNum=sizex/8*sizey;
    num=num-' ';    //得到偏移后的值
    LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置
    for(i=0; i<TypefaceNum; i++)
    {
        if(sizey==16)temp=ascii_1608[num][i];		       //调用8x16字体
        else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
        else return;
        for(t=0; t<8; t++)
        {
            if(!mode)//非叠加模式
            {
                if(temp&(0x01<<t))LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
            else//叠加模式
            {
                if(temp&(0x01<<t))TftLCD_DrawPoint(x,y,fc);//画一个点
                x++;
                if((x-x0)==sizex)
                {
                    x=x0;
                    y++;
                    break;
                }
            }
        }
    }
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void TftLCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
    while(*p!='\0')
    {
        TftLCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
        x+=sizey/2;
        p++;
    }
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
static uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}


/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void TftLCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t t,temp;
    uint8_t enshow=0;
    uint8_t sizex=sizey/2;
    for(t=0; t<len; t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                TftLCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
                continue;
            } else enshow=1;

        }
        TftLCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}


/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void TftLCD_ShowFloatNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t t,temp,sizex;
    uint16_t num1;
    sizex=sizey/2;
    num1=num*100;
    for(t=0; t<len; t++)
    {
        temp=(num1/mypow(10,len-t-1))%10;
        if(t==(len-2))
        {
            TftLCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
            t++;
            len+=1;
        }
        TftLCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}


/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组
      返回值：  无
******************************************************************************/
void TftLCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[])
{
    uint16_t i,j,k=0;
    LCD_Address_Set(x,y,x+length-1,y+width-1);
    for(i=0; i<length; i++)
    {
        for(j=0; j<width; j++)
        {
            LCD_WR_DATA8(pic[k*2]);
            LCD_WR_DATA8(pic[k*2+1]);
            k++;
        }
    }
}

