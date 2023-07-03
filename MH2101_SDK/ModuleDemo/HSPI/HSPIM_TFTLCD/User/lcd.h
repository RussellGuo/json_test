#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C"
{
#endif
    
#include "mhscpu.h"    

/********************************** Size Define *******************************/    
#define TFT_LCD_WIDTH                 240
#define TFT_LCD_HIGHT                 320

#define TFT_LCD_FONT_WIDTH            240
#define TFT_LCD_FONT_HIGHT            320
    
/********************************** Color Define ******************************/
#define TFT_LCD_COLOR_WHITE           0xFFFF
#define TFT_LCD_COLOR_BLACK           0x0000	  
#define TFT_LCD_COLOR_BLUE         	  0x001F  
#define TFT_LCD_COLOR_BRED            0xF81F
#define TFT_LCD_COLOR_GRED 			  0xFFE0
#define TFT_LCD_COLOR_GBLUE			  0x07FF
#define TFT_LCD_COLOR_RED             0xF800
#define TFT_LCD_COLOR_MAGENTA         0xF81F
#define TFT_LCD_COLOR_GREEN           0x07E0
#define TFT_LCD_COLOR_CYAN            0x7FFF
#define TFT_LCD_COLOR_YELLOW          0xFFE0
#define TFT_LCD_COLOR_BROWN 	      0xBC40
#define TFT_LCD_COLOR_BRRED 		  0xFC07
#define TFT_LCD_COLOR_GRAY  		  0x8430

/********************************* Command Define ****************************/
#define TFT_LCD_CMD_SLPOUT            0x11      ///< Sleep Out
#define TFT_LCD_CMD_GAMMASET          0x26      ///< GAMMA Set
#define TFT_LCD_CMD_DISPOFF           0x28      ///< Display Off
#define TFT_LCD_CMD_DISPON            0x29      ///< Display On
#define TFT_LCD_CMD_CASET             0x2A      ///< Column Address Set
#define TFT_LCD_CMD_RASET             0x2B      ///< Row Address Set
#define TFT_LCD_CMD_RAMWR             0x2C      ///< Memory Write
#define TFT_LCD_CMD_MEMACCESSCTRL     0x36      ///< Memory Access Control
#define TFT_LCD_CMD_COLMOD            0x3A      ///< COLMOD
#define TFT_LCD_CMD_FRAMERATECTRL     0xB1      ///< Frame Rate Control
#define TFT_LCD_CMD_DISFUNCCTRL       0xB6      ///< Display Function Control
#define TFT_LCD_CMD_PWCTRL1           0xC0      ///< Power Control 1
#define TFT_LCD_CMD_PWCTRL2           0xC1      ///< Power Control 2
#define TFT_LCD_CMD_VCOMCTRL1         0xC5      ///< VCOM Control 1
#define TFT_LCD_CMD_VCOMCTRL2         0xC7      ///< VCOM Control 2
#define TFT_LCD_CMD_PGAMCTR           0xE0      ///< Positive Gamma Correction
#define TFT_LCD_CMD_NGAMCTRL          0xE1      ///< Negative Gamma Correction
#define TFT_LCD_CMD_IFCTL             0xF6      ///< Interface Control

/********************************** Pin Define ******************************/
#define TFT_LCD_CS_PIN_GROUP          GPIOC 
#define TFT_LCD_CS_PIN_NUM            GPIO_Pin_8

#define TFT_LCD_RST_PIN_GROUP         GPIOD
#define TFT_LCD_RST_PIN_NUM           GPIO_Pin_6

#define TFT_LCD_DC_PIN_GROUP          GPIOD
#define TFT_LCD_DC_PIN_NUM            GPIO_Pin_7


#define TFT_LCD_CS_LOW()              GPIO_ResetBits(TFT_LCD_CS_PIN_GROUP, TFT_LCD_CS_PIN_NUM)
#define TFT_LCD_CS_HIGH()             GPIO_SetBits(TFT_LCD_CS_PIN_GROUP, TFT_LCD_CS_PIN_NUM)

#define TFT_LCD_RST_LOW()             GPIO_ResetBits(TFT_LCD_RST_PIN_GROUP, TFT_LCD_RST_PIN_NUM)
#define TFT_LCD_RST_HIGH()            GPIO_SetBits(TFT_LCD_RST_PIN_GROUP, TFT_LCD_RST_PIN_NUM)

#define TFT_LCD_DC_LOW()              GPIO_ResetBits(TFT_LCD_DC_PIN_GROUP, TFT_LCD_DC_PIN_NUM)
#define TFT_LCD_DC_HIGH()             GPIO_SetBits(TFT_LCD_DC_PIN_GROUP, TFT_LCD_DC_PIN_NUM)

#define TFT_LCD_REFRESH_LINES         8
#define TFT_LCD_DMA_BLOCK_SIZE        (TFT_LCD_WIDTH * 10)
/********************************** Function Define ******************************/
typedef struct
{
    uint16_t x;
    uint16_t y;
}POINT;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint8_t radius;
}CIRCLE;

typedef struct
{
    uint16_t left;
    uint16_t top;
    uint16_t right;
    uint16_t bottom;
}RECT;


extern const uint8_t ascii_1608[1520];

void TFT_LCD_Init(void);
void TFT_LCD_Clear(uint16_t Color);

void TFT_LCD_DrawPoint(POINT *point, uint16_t color);
void TFT_LCD_DrawPointBig(uint16_t x, uint16_t y, uint16_t color);

void TFT_LCD_DrawLine(POINT *startPoint, POINT *endPoint, uint16_t color);
void TFT_LCD_DrawRectangle(RECT *rect, uint16_t color);	   
void TFT_LCD_Fill(RECT *rect, uint16_t color);

void TFT_LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t mode, uint16_t backColor, uint16_t pointColor);
void TFT_LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint16_t backColor, uint16_t pointColor);
void TFT_LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t backColor, uint16_t pointColor);
void TFT_LCD_ShowImageDirect(void);
void TFT_LCD_ShowImageDMA(void);

#ifdef __cplusplus
}
#endif
        
#endif   ///< __TFT_LCD_H__
