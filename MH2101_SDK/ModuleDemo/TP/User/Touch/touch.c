#include <stdio.h>
#include "mhscpu.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"



#define GPIO_AD_MAXVALUE   3430

/*
//引脚连接
*       RS/A0(4 WIRE)    -->    SPI CLK
*       SPI SDI/SDA      -->    SPI MOSI
*       SPI SDO          -->    SPI MISO
*
                                     (Y1/YD) PE6
*                                      |
*                            +---------|----------+
*                            |         |          |
*                            |      +==|==+       |
*                            |      |     |       |     
*                            |  +===============+ |       
*            PA7 (X1/XL)<--|--|      R1       |-|-->(X2/XR) PC0/AD[1]
*                            |  +===============+ |       
*                            |      | R2  |       |       
*                            |      |     |       |       
*                            |      +==|==+       |       
*                            +=========|==========+     
*                                      |
*                                     (Y2/YU) PC5/AD[6]
*/


TP_Param_t TP_Param;


void TP_AdcInit(void)
{
    ADC_InitTypeDef ADC_InitStruct;	
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_ADC, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_ADC, ENABLE);
    
    GPIO_PullUpCmd(GPIOC, GPIO_Pin_0 | GPIO_Pin_5, DISABLE);
    
    ADC_InitStruct.ADC_Channel = ADC_CHANNEL_1;
    ADC_InitStruct.ADC_SampSpeed = ADC_SpeedPrescaler_2;
    ADC_InitStruct.ADC_IRQ_EN = DISABLE;
    ADC_InitStruct.ADC_FIFO_EN = DISABLE;

    ADC_Init(&ADC_InitStruct);

    ADC_StartCmd(ENABLE);
    ADC_BuffCmd(ENABLE);
    ADC_DivResistorCmd(ENABLE);	
}

/******************************************************************************
      函数说明：在指定位置画一个大点(2*2的点)
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void TP_DrawBigPoint(uint16_t x, uint16_t y, uint16_t color)
{	    
	TftLCD_DrawPoint(x, y, color);           //中心点 
	TftLCD_DrawPoint(x+1, y, color);
	TftLCD_DrawPoint(x, y+1, color);
	TftLCD_DrawPoint(x+1, y+1, color);	 	  	
}

void TP_DrowTouchPoint(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    TftLCD_DrawLine(x-2*r,y,x+1+2*r,y,color);
    TftLCD_DrawLine(x,y-2*r,x,y+1+2*r,color);
    TftLCD_DrawPoint(x+1,y+1,color);
    TftLCD_DrawPoint(x-1,y+1,color);
    TftLCD_DrawPoint(x+1,y-1,color);
    TftLCD_DrawPoint(x-1,y-1,color);
    TftLCD_DrawCircle(x,y,r,color);
}

uint8_t TP_IsPressed(void)
{
    uint16_t AdcPress = 0;  

	GPIO_InitTypeDef GPIO_InitStructure;  

	/* YLow output low */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = TOUCH_Y_MINUS_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
    GPIO_Init(TOUCH_Y_MINUS_GPIOx, &GPIO_InitStructure);	

	/* XLow IPU */	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = TOUCH_X_MINUS_PIN;
    GPIO_Init(TOUCH_X_MINUS_GPIOx, &GPIO_InitStructure);
		
	/* YHigh output high */
	GPIO_FuncConfig(TOUCH_Y_PLUS_GPIOx, TOUCH_Y_PLUS_PIN, GPIO_FUNC_DIG);	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = TOUCH_Y_PLUS_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
    GPIO_Init(TOUCH_Y_PLUS_GPIOx, &GPIO_InitStructure);
	
    GPIO_SetBits(TOUCH_Y_PLUS_GPIOx, TOUCH_Y_PLUS_PIN);
    GPIO_ResetBits(TOUCH_Y_MINUS_GPIOx, TOUCH_Y_MINUS_PIN);
	
	GPIO_FuncConfig(TOUCH_X_PLUS_GPIOx, TOUCH_X_PLUS_PIN, GPIO_FUNC_ANA);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = TOUCH_X_PLUS_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
    GPIO_Init(TOUCH_X_PLUS_GPIOx, &GPIO_InitStructure); 

	ADC0->ADC_CR1 = (ADC0->ADC_CR1 & ~(ADC_CR1_CHANNEL_MASK)) | TOUCH_X_ADC_CHANNEL;
	udelay(50);   //等待上面的设置稳定   

	ADC_GetResult();

    AdcPress = ADC_GetResult();
    if ((AdcPress + 3)  < GPIO_AD_MAXVALUE)
	{
		return 0;
	}
	
	return 1;	
}

uint16_t TP_GetCoordVal(uint8_t coord)
{
    uint16_t adcValue[TOUCH_CALIB_SAMP_CNT] = {0}, tempxy = 0;
    uint8_t i = 0, min = 0, j = 0;
	uint32_t sum;
  
    if (coord == COORD_X)
    {
		GPIO_InitTypeDef GPIO_InitStructure;
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = TOUCH_X_MINUS_PIN;
		GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
		GPIO_Init(TOUCH_X_MINUS_GPIOx, &GPIO_InitStructure);		
		
		GPIO_FuncConfig(TOUCH_Y_PLUS_GPIOx, TOUCH_Y_PLUS_PIN, GPIO_FUNC_DIG);		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = TOUCH_Y_PLUS_PIN;
		GPIO_Init(TOUCH_Y_PLUS_GPIOx, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = TOUCH_Y_MINUS_PIN;
		GPIO_Init(TOUCH_Y_MINUS_GPIOx, &GPIO_InitStructure);		

		GPIO_SetBits(TOUCH_Y_PLUS_GPIOx, TOUCH_Y_PLUS_PIN);
		GPIO_ResetBits(TOUCH_Y_MINUS_GPIOx, TOUCH_Y_MINUS_PIN);	
		
		GPIO_FuncConfig(TOUCH_X_PLUS_GPIOx, TOUCH_X_PLUS_PIN, GPIO_FUNC_ANA);		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = TOUCH_X_PLUS_PIN;
		GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
		GPIO_Init(TOUCH_X_PLUS_GPIOx, &GPIO_InitStructure);	

		ADC0->ADC_CR1 = (ADC0->ADC_CR1 & ~(ADC_CR1_CHANNEL_MASK)) | TOUCH_X_ADC_CHANNEL;
		udelay(50);   //等待上面的设置稳定 	
	
		ADC_GetResult();
        for (i = 0; i < TOUCH_CALIB_SAMP_CNT; i++)
        {
            adcValue[i] = ADC_GetResult();
			CALIB_DEBUG("Coord[X]: %d\n", adcValue[i]);
        }
    }
    else if (coord == COORD_Y)
    {
		GPIO_InitTypeDef GPIO_InitStructure;
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = TOUCH_Y_MINUS_PIN;
		GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
		GPIO_Init(TOUCH_Y_MINUS_GPIOx, &GPIO_InitStructure);			

		GPIO_FuncConfig(TOUCH_X_PLUS_GPIOx, TOUCH_X_PLUS_PIN, GPIO_FUNC_DIG);			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = TOUCH_X_PLUS_PIN;
		GPIO_Init(TOUCH_X_PLUS_GPIOx, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = TOUCH_X_MINUS_PIN;
		GPIO_Init(TOUCH_X_MINUS_GPIOx, &GPIO_InitStructure);		
			
		GPIO_ResetBits(TOUCH_X_PLUS_GPIOx, TOUCH_X_PLUS_PIN);
		GPIO_SetBits(TOUCH_X_MINUS_GPIOx, TOUCH_X_MINUS_PIN);	
		
		GPIO_FuncConfig(TOUCH_Y_PLUS_GPIOx, TOUCH_Y_PLUS_PIN, GPIO_FUNC_ANA);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = TOUCH_Y_PLUS_PIN;
		GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;		
		GPIO_Init(TOUCH_Y_PLUS_GPIOx, &GPIO_InitStructure);
		
		ADC0->ADC_CR1 = (ADC0->ADC_CR1 & ~(ADC_CR1_CHANNEL_MASK)) | TOUCH_Y_ADC_CHANNEL;  
		udelay(50);   //等待上面的设置稳定   
		
		ADC_GetResult();
        for (i = 0; i < TOUCH_CALIB_SAMP_CNT; i++)
        {
            adcValue[i] = ADC_GetResult();
			CALIB_DEBUG("Coord[Y]: %d\n", adcValue[i]);
        }
    }

    for (i = 0; i < TOUCH_CALIB_SAMP_CNT - 1; i++)
    {
        min = i;
        for (j = i + 1; j < TOUCH_CALIB_SAMP_CNT; j++)
        {
            if (adcValue[min] > adcValue[j])
			{
				min = j;
			}
        }
        tempxy = adcValue[i];
        adcValue[i] = adcValue[min];
        adcValue[min] = tempxy;
    }
	
	sum = 0;
	for (i = 2; i < TOUCH_CALIB_SAMP_CNT-TOUCH_CALIB_DROP_NUM; i++)
	{
		sum += adcValue[i];	
	}

	tempxy = sum / (TOUCH_CALIB_SAMP_CNT - 2*TOUCH_CALIB_DROP_NUM);
	
    return tempxy;
}

uint8_t TP_Calibrate(void)
{
    uint8_t cnt = 0, difference_x1 = 0, difference_x2 = 0, difference_y1 = 0, difference_y2 = 0;
    uint32_t time_cnt = 0;
    uint16_t x, y;
	Point_t lcoord_point[5];  // logic coord
	Point_t calib_point[5] = 
	{
		{30, 30},
		{30, 210},
		{290, 30},
		{290, 210},
		{160, 120},
	};
	
	/* Draw the first circle */
    TP_DrowTouchPoint(calib_point[0].x, calib_point[0].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);
	TP_DrawBigPoint(calib_point[0].x, calib_point[0].y,RED);
	
    while (1)
    {
        if (TP_IsPressed() == 0)
        {
            x = TP_GetCoordVal(COORD_X);
            y = TP_GetCoordVal(COORD_Y);
            while (TP_IsPressed() == 0);
            cnt++;
            
            switch (cnt)
            {
                case 1:
                {
                    lcoord_point[0].x = x;
                    lcoord_point[0].y = y;
                    CALIB_DEBUG("p1(x, y) = (%d, %d)\n", lcoord_point[0].x, lcoord_point[0].y);
                    TP_DrowTouchPoint(calib_point[0].x, calib_point[0].y, TOUCH_CALIB_CIRCLE_RADIUS, WHITE);    /* Clear the first circle */
                    TP_DrawBigPoint(calib_point[0].x, calib_point[0].y,WHITE);
                    TP_DrowTouchPoint(calib_point[1].x, calib_point[1].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);      /* Draw the second circle */			
                    TP_DrawBigPoint(calib_point[1].x, calib_point[1].y,RED);
                }
                break;
                
                case 2:
                {
                    lcoord_point[1].x = x;
                    lcoord_point[1].y = y;
                    CALIB_DEBUG("p2(x, y) = (%d, %d)\n", lcoord_point[1].x, lcoord_point[1].y);
                    TP_DrowTouchPoint(calib_point[1].x, calib_point[1].y, TOUCH_CALIB_CIRCLE_RADIUS, WHITE);    /* Clear the second circle */
                    TP_DrawBigPoint(calib_point[1].x, calib_point[1].y,WHITE);
                    TP_DrowTouchPoint(calib_point[2].x, calib_point[2].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);      /* Draw the third circle */			
                    TP_DrawBigPoint(calib_point[2].x, calib_point[2].y,RED);
                }
                break;
                
                case 3:
                {
                    lcoord_point[2].x = x;
                    lcoord_point[2].y = y;
                    CALIB_DEBUG("p3(x, y) = (%d, %d)\n", lcoord_point[2].x, lcoord_point[2].y);
                    TP_DrowTouchPoint(calib_point[2].x, calib_point[2].y, TOUCH_CALIB_CIRCLE_RADIUS, WHITE);    /* Clear the third circle */
                    TP_DrawBigPoint(calib_point[2].x, calib_point[2].y,WHITE);
                    TP_DrowTouchPoint(calib_point[3].x, calib_point[3].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);	    /* Draw the fourth circle */	 
                    TP_DrawBigPoint(calib_point[3].x, calib_point[3].y,RED);
                }
                break;
                
                case 4:
                {
                    lcoord_point[3].x = x;
                    lcoord_point[3].y = y;
                    CALIB_DEBUG("p4(x, y) = (%d, %d)\n", lcoord_point[3].x, lcoord_point[3].y);
                    TP_DrowTouchPoint(calib_point[3].x, calib_point[3].y, TOUCH_CALIB_CIRCLE_RADIUS, WHITE);    /* Clear the fourth circle */
                    TP_DrawBigPoint(calib_point[3].x, calib_point[3].y,WHITE);
                    TP_DrowTouchPoint(calib_point[4].x, calib_point[4].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);		/* Draw the fifth circle */
                    TP_DrawBigPoint(calib_point[4].x, calib_point[4].y,RED);
                }
                break;
                
                case 5:
                {
                    lcoord_point[4].x = x;
                    lcoord_point[4].y = y;
                    CALIB_DEBUG("p5(x, y) = (%d, %d)\n", lcoord_point[4].x, lcoord_point[4].y);
                    TP_DrowTouchPoint(calib_point[4].x, calib_point[4].y, TOUCH_CALIB_CIRCLE_RADIUS, WHITE);		      /* Clear the fifth circle */	
                    TP_DrawBigPoint(calib_point[4].x, calib_point[4].y,WHITE);
                    
                    difference_x1 = (lcoord_point[1].x > lcoord_point[0].x) ? (lcoord_point[1].x - lcoord_point[0].x) : (lcoord_point[0].x - lcoord_point[1].x);
                    difference_x2 = (lcoord_point[2].x > lcoord_point[3].x) ? (lcoord_point[2].x - lcoord_point[3].x) : (lcoord_point[3].x - lcoord_point[2].x);
                    
                    difference_y1 = (lcoord_point[2].y > lcoord_point[0].y) ? (lcoord_point[2].y - lcoord_point[0].y) : (lcoord_point[0].y - lcoord_point[2].y);
                    difference_y2 = (lcoord_point[1].y > lcoord_point[3].y) ? (lcoord_point[1].y - lcoord_point[3].y) : (lcoord_point[3].y - lcoord_point[1].y);

                    CALIB_DEBUG("difference_x1 = %d\n", difference_x1);
                    CALIB_DEBUG("difference_x2 = %d\n", difference_x2);

                    CALIB_DEBUG("difference_y1 = %d\n", difference_y1);
                    CALIB_DEBUG("difference_y2 = %d\n", difference_y2);				
                    
                    if ((difference_x1 < TOUCH_CALIB_ACCURACY) && (difference_x2 < TOUCH_CALIB_ACCURACY) \
                         && (difference_y1 < TOUCH_CALIB_ACCURACY) && (difference_y2 < TOUCH_CALIB_ACCURACY))
                    {
                        // calc KX and KY
                        TP_Param.xfac = (((lcoord_point[2].x - lcoord_point[0].x) * 100) / (calib_point[2].x - calib_point[0].x) + ((lcoord_point[3].x - lcoord_point[1].x) * 100) / (calib_point[3].x - calib_point[1].x)) / 2;
                        TP_Param.yfac = (((lcoord_point[1].y - lcoord_point[0].y) * 100) / (calib_point[1].y - calib_point[0].y) + ((lcoord_point[3].y - lcoord_point[2].y) * 100) / (calib_point[3].y - calib_point[2].y)) / 2;

                        CALIB_DEBUG("TP_Param.xfac = %d\n", TP_Param.xfac);
                        CALIB_DEBUG("TP_Param.yfac = %d\n", TP_Param.yfac);					
                        
                        //反算中间点的逻辑坐标
                        x = lcoord_point[0].x + (TP_Param.xfac * (calib_point[4].x - calib_point[0].x)) / 100;
                        y = lcoord_point[0].y + (TP_Param.yfac * (calib_point[4].y - calib_point[0].y)) / 100;

                        CALIB_DEBUG("(x, y) = (%d, %d)\n", x, y);				
                        
                        difference_x1 = (x > lcoord_point[4].x) ? (x - lcoord_point[4].x) : (lcoord_point[4].x - x);
                        difference_y1 = (y > lcoord_point[4].y) ? (y - lcoord_point[4].y) : (lcoord_point[4].y - y);
                        
                        CALIB_DEBUG("difference_x1 = %d\n", difference_x1);
                        CALIB_DEBUG("difference_y1 = %d\n", difference_y1);
                        
                        if (difference_x1 < TOUCH_CALIB_ACCURACY && difference_y1 < TOUCH_CALIB_ACCURACY)
                        {
                            //物理坐标零点
                            TP_Param.xbase = calib_point[0].x;
                            TP_Param.ybase = calib_point[0].y;
                            //逻辑坐标零点
                            TP_Param.xlogic = lcoord_point[0].x;
                            TP_Param.ylogic = lcoord_point[0].y;

                            return 1;
                        }
                        else
                        {
                            cnt = 0;
                        }
                    }
                    else
                    {
                        cnt = 0;    
                    }
                    
                    time_cnt++;
                    if (time_cnt >= 3)
                    {
                        TP_Param.xfac = 102;
                        TP_Param.yfac = 84;
                        TP_Param.xbase = 30;
                        TP_Param.ybase = 30;
                        TP_Param.xlogic = 29;
                        TP_Param.ylogic = 83;

                        return 0;

                    }
                    
                    TP_DrowTouchPoint(calib_point[0].x, calib_point[0].y, TOUCH_CALIB_CIRCLE_RADIUS, RED);
                    TP_DrawBigPoint(calib_point[0].x, calib_point[0].y,RED);
                }
                break;
            }
        }
    }
    return 0;
}

uint8_t TP_Scan(Point_t *tp)
{
    uint16_t x, y;
    
    if (TP_IsPressed() == 0)
    {
        //获取逻辑坐标
        x = TP_GetCoordVal(COORD_X);
        y = TP_GetCoordVal(COORD_Y);
        
        //换算成物理坐标
        //x = lcoord_point[0].x + (TP_Param.xfac * (calib_point[4].x - calib_point[0].x)) / 100;
        tp->x = ((x-TP_Param.xlogic)*100)/TP_Param.xfac + TP_Param.xbase;
        tp->y = ((y-TP_Param.ylogic)*100)/TP_Param.yfac + TP_Param.ybase;
        
        return 0;
    }
    else
    {
        return 1;
    }  
}
