#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "mhscpu.h"


#define TFTLCD_WIDTH      320
#define TFTLCD_HEIGHT     240


//-----------------LCD端口定义---------------- 
#define LCD_RESET_GPIOx        GPIOD
#define LCD_RESET_PIN          GPIO_Pin_6

#define LCD_DC_GPIOx           GPIOD
#define LCD_DC_PIN             GPIO_Pin_7

#define LCD_CS_GPIOx           GPIOC
#define LCD_CS_PIN             GPIO_Pin_14

#define LCD_BL_GPIOx           GPIOA
#define LCD_BL_PIN             GPIO_Pin_2

#define LCD_RES_Clr()          GPIO_ResetBits(LCD_RESET_GPIOx, LCD_RESET_PIN)  //RES
#define LCD_RES_Set()          GPIO_SetBits(LCD_RESET_GPIOx, LCD_RESET_PIN)

#define LCD_DC_Clr()           GPIO_ResetBits(LCD_DC_GPIOx, LCD_DC_PIN)  //DC
#define LCD_DC_Set()           GPIO_SetBits(LCD_DC_GPIOx, LCD_DC_PIN)

#define LCD_CS_Clr()           GPIO_ResetBits(LCD_CS_GPIOx, LCD_CS_PIN)  //CS
#define LCD_CS_Set()           GPIO_SetBits(LCD_CS_GPIOx, LCD_CS_PIN)

#define LCD_BL_On()            GPIO_SetBits(LCD_BL_GPIOx, LCD_BL_PIN)  //backlight
#define LCD_BL_Off()           GPIO_ResetBits(LCD_BL_GPIOx, LCD_BL_PIN)


#define LCD_SPI                HSPIM0

#define TFT_LCD_DMA_BLOCK_SIZE        (TFTLCD_WIDTH * 10)

void LCD_GPIO_Init(void);
void LCD_SPI_Init(void); 

void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节

void LCD_WR_REG(uint8_t dat);//写入一个指令

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);//设置坐标函数

void LCD_Init(void);//LCD初始化

#endif
