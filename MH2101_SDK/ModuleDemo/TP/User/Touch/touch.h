#ifndef  __TOUCH_H
#define  __TOUCH_H

#include "mhscpu.h"


#define TOUCH_X_PLUS_GPIOx            GPIOC
#define TOUCH_X_PLUS_PIN              GPIO_Pin_0
#define TOUCH_X_MINUS_GPIOx           GPIOA
#define TOUCH_X_MINUS_PIN             GPIO_Pin_7

#define TOUCH_Y_PLUS_GPIOx            GPIOC
#define TOUCH_Y_PLUS_PIN              GPIO_Pin_5
#define TOUCH_Y_MINUS_GPIOx           GPIOE
#define TOUCH_Y_MINUS_PIN             GPIO_Pin_6

#define TOUCH_X_ADC_CHANNEL           ADC_CHANNEL_1
#define TOUCH_Y_ADC_CHANNEL           ADC_CHANNEL_6

#define COORD_X                       0
#define COORD_Y                       1

#define TOUCH_CALIB_CIRCLE_RADIUS     6

#define TOUCH_CALIB_SAMP_CNT          10
#define TOUCH_CALIB_DROP_NUM          2

#define TOUCH_CALIB_ACCURACY          40
                                                                   
//触摸屏校准参数 ==
#define  TOUCH_SCR_H_SIZE             240                 
#define  TOUCH_SCR_V_SIZE             320                 
 
 
#define CALIB_DEBUG(format, args...)  //printf(format, ##args)    


typedef struct
{
	uint16_t x;
	uint16_t y;
}Point_t;
                                                          
typedef struct
{ 
    uint16_t xbase; 
    uint16_t ybase; 
    uint16_t xlogic; 
    uint16_t ylogic;
    uint16_t xfac;
    uint16_t yfac;
}TP_Param_t;


void TP_AdcInit(void);

uint8_t TP_IsPressed(void);

uint16_t TP_GetCoordVal(uint8_t coord);

uint8_t TP_Calibrate(void);

uint8_t TP_Scan(Point_t *tp);

void TP_DrawBigPoint(uint16_t x, uint16_t y, uint16_t color);

#endif     ///< __TOUCH_H
