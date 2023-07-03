/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : lcd.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 9-November-2019
 *****************************************************************************/
 
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mhscpu.h"

#define	LCD_CS_PORT			GPIOC
#define LCD_CS_PIN			GPIO_Pin_5
#define	SET_LCD_CS_LOW()	GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN)
#define	SET_LCD_CS_HIGH()	GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN)

#define LCD_RST_PORT		GPIOC
#define LCD_RST_PIN			GPIO_Pin_6
#define	SET_LCD_RST_LOW()	GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN)
#define	SET_LCD_RST_HIGH()	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN)

#define LCD_A0_PORT			GPIOC
#define LCD_A0_PIN			GPIO_Pin_7
#define	SET_LCD_A0_LOW()	GPIO_ResetBits(LCD_A0_PORT, LCD_A0_PIN)
#define	SET_LCD_A0_HIGH()	GPIO_SetBits(LCD_A0_PORT, LCD_A0_PIN)

#define LCD_LIGHT_PORT		GPIOC
#define LCD_LIGHT_PIN		GPIO_Pin_8

#define  nop()      {int i; for (i = 0; i < 5; i++);}   /*定义空指令*/

extern HSPIM_TypeDef *HSPIx;

void lcd_init(void);
void delay(void);
void delay1(void);  

void wcom(unsigned char com);
void wdata(unsigned char dat);
void write(unsigned char a);


void disdate_a(unsigned char dat_a,unsigned char dat_b);
void disdate_b(const unsigned char *p);
void cwpr(unsigned char cod,unsigned char page,unsigned char seg);
void ccwpr(unsigned char cod,unsigned char page,unsigned char seg);

/*  西文字符库定义  */
const unsigned char success[768] =
{ 
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,    /*" "=00H*/
	0x00,0x00,0x00,0x4F,0x00,0x00,0x00,0x00,    /*"!"=01H*/
	0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00,    /*"""=02h*/
	0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00,    /*"#"=03h*/
	0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00,    /*"$"=04h*/
	0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00,    /*"%"=05h*/
	0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00,    /*"&"=06h*/
	0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00,    /*"'"=07h*/
	0x00,0x00,0x1C,0x22,0x41,0x00,0x00,0x00,    /*"("=08h*/
	0x00,0x00,0x41,0x22,0x1C,0x00,0x00,0x00,    /*")"=09h*/
	0x00,0x14,0x08,0x3E,0x08,0x14,0x00,0x00,    /*"*"=0Ah*/
	0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00,    /*"+"=0Bh*/
	0x00,0x00,0x50,0x30,0x00,0x00,0x00,0x00,    /*";"=0Ch*/
	0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00,    /*"-"=0Dh*/
	0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00,    /*"."=0Eh*/
	0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00,    /*"/"=0Fh*/
	0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00,    /*"0"=10h*/
	0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00,    /*"1"=11h*/
	0x00,0x42,0x61,0x51,0x49,0x46,0x00,0x00,    /*"2"=12h*/
	0x00,0x21,0x41,0x45,0x4B,0x31,0x00,0x00,    /*"3"=13h*/
	0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00,    /*"4"=14h*/
	0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00,    /*"5"=15h*/
	0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00,    /*"6"=16h*/
	0x00,0x01,0x01,0x79,0x05,0x03,0x00,0x00,    /*"7"=17h*/
	0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00,    /*"8"=18h*/
	0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00,    /*"9"=19h*/
	0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00,    /*":"=1Ah*/
	0x00,0x00,0x56,0x36,0x00,0x00,0x00,0x00,    /*";"=1Bh*/
	0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00,    /*"<"=1Ch*/
	0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00,    /*"="=1Dh*/
	0x00,0x00,0x41,0x22,0x14,0x08,0x00,0x00,    /*">"=1Eh*/
	0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00,    /*"?"=1Fh*/
	0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00,    /*"@"=20h*/
	0x00,0x7E,0x11,0x11,0x11,0x7E,0x00,0x00,    /*"A"=21h*/
	0x00,0x41,0x7F,0x49,0x49,0x36,0x00,0x00,    /*"B"=22h*/
	0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00,    /*"C"=23h*/
	0x00,0x41,0x7F,0x41,0x41,0x3E,0x00,0x00,    /*"D"=24h*/
	0x00,0x7F,0x49,0x49,0x49,0x49,0x00,0x00,    /*"E"=25h*/
	0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00,    /*"F"=26h*/
	0x00,0x3E,0x41,0x41,0x49,0x7A,0x00,0x00,    /*"G"=27h*/
	0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00,    /*"h"=28h*/
	0x00,0x00,0x41,0x7F,0x41,0x00,0x00,0x00,    /*"I"=29h*/
	0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00,    /*"J"=2Ah*/
	0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00,    /*"K"=2Bh*/
	0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00,    /*"L"=2Ch*/
	0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00,    /*"M"=2Dh*/
	0x00,0x7F,0x06,0x08,0x30,0x7F,0x00,0x00,    /*"N"=2Eh*/
	0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00,    /*"O"=2Fh*/
	0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00,    /*"P"=30h*/
	0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00,    /*"Q"=31h*/
	0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00,    /*"R"=32h*/
	0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00,    /*"S"=33h*/
	0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00,    /*"T"=34h*/
	0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00,    /*"U"=35h*/
	0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00,    /*"V"=36h*/
	0x00,0x7F,0x20,0x18,0x20,0x7F,0x00,0x00,    /*"W"=37h*/
	0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00,    /*"X"=38h*/
	0x00,0x07,0x08,0x70,0x08,0x07,0x00,0x00,    /*"Y"=39h*/
	0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00,    /*"Z"=3Ah*/
	0x00,0x00,0x7F,0x41,0x41,0x00,0x00,0x00,    /*"["=3Bh*/
	0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00,    /*"\"=3Ch*/
	0x00,0x00,0x41,0x41,0x7F,0x00,0x00,0x00,    /*"]"=3Dh*/
	0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00,    /*"^"=3Eh*/
	0x00,0x40,0x40,0x40,0x40,0x40,0x00,0x00,    /*"_"=3Fh*/
	0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00,    /*"`"=40h*/
	0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00,    /*"a"=41h*/
	0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00,    /*"b"=42h*/
	0x00,0x38,0x44,0x44,0x44,0x28,0x00,0x00,    /*"c"=43h*/
	0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00,    /*"d"=44h*/
	0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00,    /*"e"=45h*/
	0x00,0x00,0x08,0x7E,0x09,0x02,0x00,0x00,    /*"f"=46h*/
	0x00,0x0C,0x52,0x52,0x4C,0x3E,0x00,0x00,    /*"g"=47h*/
	0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00,    /*"h"=48h*/
	0x00,0x00,0x44,0x7D,0x40,0x00,0x00,0x00,    /*"i"=49h*/
	0x00,0x20,0x40,0x44,0x3D,0x00,0x00,0x00,    /*"j"=4Ah*/
	0x00,0x00,0x7F,0x10,0x28,0x44,0x00,0x00,    /*"k"=4Bh*/
	0x00,0x00,0x41,0x7F,0x40,0x00,0x00,0x00,    /*"l"=4Ch*/
	0x00,0x7C,0x04,0x78,0x04,0x78,0x00,0x00,    /*"m"=4Dh*/
	0x00,0x7C,0x08,0x04,0x04,0x78,0x00,0x00,    /*"n"=4Eh*/
	0x00,0x38,0x44,0x44,0x44,0x38,0x00,0x00,    /*"o'=4Fh*/
	0x00,0x7E,0x0C,0x12,0x12,0x0C,0x00,0x00,    /*"p"=50h*/
	0x00,0x0C,0x12,0x12,0x0C,0x7E,0x00,0x00,    /*"q"=51h*/
	0x00,0x7C,0x08,0x04,0x04,0x08,0x00,0x00,    /*"r"=52h*/
	0x00,0x58,0x54,0x54,0x54,0x64,0x00,0x00,    /*"s"=53h*/
	0x00,0x04,0x3F,0x44,0x40,0x20,0x00,0x00,    /*"t"=54h*/
	0x00,0x3C,0x40,0x40,0x3C,0x40,0x00,0x00,    /*"u"=55h*/
	0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00,    /*"v"=56h*/
	0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00,    /*"w"=57h*/
	0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00,    /*"x"=58h*/
	0x00,0x1C,0xA0,0xA0,0x90,0x7C,0x00,0x00,    /*"y"=59h*/
	0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00,    /*"z"=5Ah*/
	0x00,0x00,0x08,0x36,0x41,0x00,0x00,0x00,    /*"{"=5Bh*/
	0x00,0x00,0x00,0x77,0x00,0x00,0x00,0x00,    /*"|"=5Ch*/
	0x00,0x00,0x41,0x36,0x08,0x00,0x00,0x00,    /*"}"=5Dh*/
	0x00,0x02,0x01,0x02,0x04,0x02,0x00,0x00,    /*"~"=5Fh*/
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00     /*" "=0x60*/
};

/*  中文字符库定义  */
const unsigned char CCTAB[128] = /* 点阵：16x16
   提取点阵方向：纵向
   字节掉转：是
   字节方式：C语言 */
{
	0x00,0x08,0x30,0x10,0x00,0xFF,0x00,0x00,    // 兆
	0x00,0xFF,0xA0,0x10,0x0C,0x08,0x00,0x00,
	0x02,0x86,0x42,0x21,0x18,0x07,0x00,0x00,
	0x00,0x3F,0x40,0x41,0x46,0x42,0x70,0x00,	

	0x20,0x21,0x2E,0xE4,0x00,0x42,0x42,0xFE,    // 讯
	0x42,0x42,0x42,0x02,0xFE,0x00,0x00,0x00,
	0x00,0x00,0x00,0x7F,0x20,0x10,0x00,0x7F,
	0x00,0x00,0x00,0x00,0x3F,0x40,0x38,0x00,

	0x70,0x00,0xFF,0x08,0x30,0x02,0xF2,0x92,    // 恒
	0x92,0x92,0x92,0x92,0xF2,0x02,0x00,0x00,
	0x00,0x00,0x7F,0x00,0x20,0x20,0x27,0x24,
	0x24,0x24,0x24,0x24,0x27,0x20,0x20,0x00,

	0x80,0x82,0x84,0x0C,0x10,0x10,0x10,0x10,    // 达
	0xFE,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
	0x40,0x20,0x1F,0x20,0x50,0x48,0x44,0x43,
	0x40,0x41,0x42,0x44,0x5C,0x48,0x20,0x00,
};

const unsigned char gImagedog[1024] =  /* 0X22,0X01,0X80,0X00,0X60,0X00, */
{
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0X7F,0X7F,0X7F,0X7F,0X7F,0X3F,0X3F,0X3F,0X3F,0X3F,0X3F,0X1F,
	0X3F,0X3F,0X3F,0X3F,0X7F,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0X7F,0X6F,0X8F,0X07,0X03,0X03,0X03,0X03,0X03,0X03,0X03,
	0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X03,0X07,0X07,0X0F,0X1F,
	0X3F,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,
	0X1F,0XAF,0XFF,0XC7,0X53,0XF9,0XFF,0XFF,0XF4,0XC0,0X00,0X00,0X80,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X01,0X03,0X03,0X07,0X0F,0X0F,0X0F,0X2F,0X2F,0X7F,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XFE,
	0XC3,0X0B,0XFF,0X8F,0X9F,0X1E,0X7F,0X3F,0X1F,0X17,0X00,0XD8,0XFD,0X00,0X00,0X00,
	0X10,0X38,0XF0,0XF4,0XFC,0X7C,0XFC,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X3C,0X7C,0X78,0X30,0X18,0X30,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0D,0XFD,
	0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XE3,0XE7,0X97,0XFF,0X7E,0XFF,0XFC,0XE4,0XCC,0X40,0XFF,0XFD,0XC0,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X30,0X3F,
	0X1F,0X0F,0X1F,0X1F,0X1F,0X1F,0X0F,0X1F,0X3F,0X7F,0X7F,0X7F,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0X01,0X03,0X03,0X03,0X07,0X07,0X03,0X03,0X03,0X03,0X01,0X01,0X01,0X01,0X01,0X01,
	0X07,0X07,0X07,0X03,0X00,0X00,0X01,0X06,0X07,0X0B,0X1E,0X3F,0X3F,0X7F,0XFC,0XE0,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFC,0XFC,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFE,0X7C,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XFC,0XE0,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X03,
	0X03,0X03,0X03,0X01,0X03,0X03,0X0F,0X1F,0X3F,0X3F,0X3F,0X3F,0X3F,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X7F,0X3F,0X7F,0X3F,0X3F,0X1F,0X3F,0X1F,0X3F,
	0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XF0,0XF8,0X80,0X00,0X00,0X00,0X00,0X10,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
	0X20,0X00,0X80,0XC0,0X00,0X80,0X00,0X80,0XC0,0X8C,0X9D,0XB7,0XAF,0XBB,0XB9,0XBF,
	0XB5,0X9F,0XDF,0XC0,0XC0,0XC0,0XC0,0XC0,0XC0,0XEC,0XF0,0XF0,0XFB,0XBD,0XAC,0X04,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X00,0X00,0XC0,0XC0,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X01,0X0F,0X07,
	0X07,0X03,0X03,0X01,0X03,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XF0,
	0X00,0X00,0X0C,0X1E,0X3F,0X7F,0X1F,0X3F,0X07,0X07,0X00,0X00,0X30,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X01,0X00,0X00,0X00,0X02,0X07,0X01,0X01,0X01,0X01,0X03,0X03,0X03,0X0F,
	0X07,0X07,0XF7,0X33,0X03,0X03,0X0F,0X0F,0XF3,0XC4,0XC3,0X53,0XD5,0XFF,0XFF,0X1F,
	0X00,0X80,0X00,0X00,0X00,0X00,0X06,0X06,0X07,0X07,0X07,0X03,0X01,0X00,0X00,0X00,
	0X80,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X0F,0X1F,0X1E,0X0E,0X08,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X07,0X07,0X07,
};


/*************************************************/
 /*延时*/
/*************************************************/
void delay(void)    
{
	unsigned char i,j;
	
	for (i = 32; i > 0; i--)
		for (j = 255; j > 0; j--);
}

 /*************************************************/
 /*延时*/
/*************************************************/
void delay1(void)    
{
	int i,j;
	for (i = 1000; i > 0; i--)
	{
		for (j = 255; j > 0; j--) 
		{
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
		}
	}
}

void lcd_spi_init()
{
	HSPIM_InitTypeDef hspim;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_15 , GPIO_Remap_3);
	
	hspim.HSPIM_Mode = HSPIM_MODE_STANDARD;
	hspim.HSPIM_CPOL = HSPIM_CPOL_LOW;
	hspim.HSPIM_CPHA = HSPIM_CPHA_1EDGE;
	hspim.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	hspim.HSPIM_DivideEnable = ENABLE;
	hspim.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_32;
	hspim.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_8;
	hspim.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_8;
	
	HSPIM_Init(HSPIx, &hspim);
	HSPIM_FIFOReset(HSPIx, HSPIM_FIFO_TX | HSPIM_FIFO_RX);	
	HSPIM_Cmd(HSPIx, ENABLE);
	HSPIM_TransmitCmd(HSPIx, ENABLE);
}
 
/*************************************************/
//初始化
/*************************************************/
void lcd_init(void)
{
	GPIO_InitTypeDef gpio;
	
	gpio.GPIO_Pin = LCD_CS_PIN;						// LCD_CS
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(LCD_CS_PORT, &gpio);
	
	gpio.GPIO_Pin = LCD_A0_PIN;						// LCD_A0
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(LCD_A0_PORT, &gpio);
	
	gpio.GPIO_Pin = LCD_RST_PIN;					// LCD_RST
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(LCD_RST_PORT, &gpio);
	
	gpio.GPIO_Pin = LCD_LIGHT_PIN;					// LCD_LIGHT
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(LCD_LIGHT_PORT, &gpio);
	
	lcd_spi_init();
	
	SET_LCD_RST_LOW();
	delay();
	SET_LCD_RST_HIGH();
	delay();
	
	wcom(0xa3);       //LCD BIAS SET ( A2 1/8;A3 1/65DUTY)
	wcom(0xa0);       //ADC  select,NORMAL   0-->127
	wcom(0xaf);       // DISPLAY ON/OFF
	wcom(0x2f);       //power control(VB,VR,VF=1,1,1)
	wcom(0x23);       // VOLT REGULATOR RADIO SET 
	wcom(0x81);       //ELECTRONIC VOLUME mode setting 100B 对比度命令
	wcom(0x28);       //REFERENCE VOLT REG  set
	wcom(0xc0);       //SHL SELECT
	wcom(0xaf);       //DISPLAY ON/OFF
	wcom(0x40);       // DISPLAY START LINE SET
}



/*************************************************/
//写命令
/*************************************************/
void wcom(unsigned char com)
{ 
	SET_LCD_CS_LOW();
	nop();
	SET_LCD_A0_LOW();
	nop();
	write(com);
	nop();
	nop(); 
	SET_LCD_CS_HIGH();
}

/*************************************************/
//写数据
/*************************************************/
void wdata(unsigned char dat)
{
	SET_LCD_CS_LOW();
	nop();
	SET_LCD_A0_HIGH();
	nop();
	write(dat);  
	nop(); 
	SET_LCD_CS_HIGH();
}

/*************************************************
//串口移位输出
*************************************************/
void write(unsigned char dat)
{  
	HSPIM_SendData(HSPIx,dat);
	//while(SET == HSPIM_GetFlagStatus(HSPIM_FLAG_TXE));
	//while(SET == HSPIM_GetFlagStatus(HSPIM_FLAG_RXE));
#if 0	
	unsigned char s,temp;
	int i;
	
	sclk = 0;
	s = dat;

	for (i = 8; i > 0; i--)
	{
		sclk = 0;
		nop();
		nop();
		temp = s & 0x80;
		if (temp)
		{
			di = 1;
		}
		else 
		{
			di = 0;
		}

		sclk = 1;
		s = s<<1;  
	}
#endif
} 

 /********************************************/
/*  西文字符写入函数:CW_PR                */

void cwpr(unsigned char cod,unsigned char page,unsigned char seg)
{
	unsigned char zmseg;

	wcom(0xaf);			//开显示
	wcom(0xb0+page);    //设定页地址                                          
	wcom(0x10+(0x0f&(seg>>4))); //设定列地址            		
	wcom(0x0f&seg);
	
	for (zmseg = 0; zmseg < 8; zmseg++)
	{
		wdata(success[cod*8+zmseg]);
	}
}

/*16*16汉字写入子程序*/
void ccwpr(unsigned char cod,unsigned char page,unsigned char seg)
{
	unsigned char zmseg;

	wcom(0xaf);				//开显示
	wcom(0xb0+page); 		//设定页地址                                            
	wcom(0x10+(0x0f&(seg>>4)));  //列地址           		
	wcom(0x0f&seg);
	
	for (zmseg = 0; zmseg <= 15; zmseg++)
	{   
		wdata(CCTAB[cod*32+zmseg]);
	}

	wcom(0xb0+page+1);  
	wcom(0x10+(0x0f&(seg>>4)));  
	wcom(0x0f&seg); 

	for (zmseg = 16; zmseg <= 31; zmseg++)
	{
		wdata(CCTAB[cod*32+zmseg]);
	}

}

/*************************************************/
//写disdate_a数据
/*************************************************/
void disdate_a(unsigned char dat_a,unsigned char dat_b)
{
	int seg;
	unsigned char page;
	
	for (page = 0xb0; page < 0xb9; page++)             //写页地址共9页  0xb8----0xb9
	{
		wcom(page);                                    //如下汉字可改
		wcom(0x10); 
		wcom(0x00); 

		for (seg = 0; seg < 64; seg++)
		{
			wdata(dat_a);
			wdata(dat_b);
		}
	} 
	wcom(0xaf); /*开显示指令*/          
}


/*************************************************/
//写disdate_b数据全屏写数据
/*************************************************/
void disdate_b(const unsigned char *p)
{
	int seg;
	unsigned char page;
	
	wcom(0xaf);/*开显示*/
	for (page = 0xb0; page < 0xb8; page++)                    //写页地址共8页  0xb0----0xb8
	{
		wcom(page);                      
		wcom(0x10); 
		wcom(0x00); 

		for (seg = 0; seg < 128; seg++)
		{ 
			wdata(*p++);
		}              
	}           
 }

/*************************************************/
//写disdate_c数据
/*************************************************/
/*void disdate_c(unsigned char *q)
{ 
	int seg;
	unsigned char page;
 
	wcom(0xaf);
	for (page = 0xb0; page < 0xb9; page++) //写页地址共8页  0xb0----0xb8
	{
		wcom(page);
		wcom(0x10);
		wcom(0x00);
		q=q+128;
		for (seg = 128; seg > 0; seg--)
		{
			wdata(*q--);
		}
 
		q = q+128;        
	}
}    */       


/*主程序
*************************************************/
void LCD_Test(void)
{	 
	unsigned char i,j,n;
	unsigned char k = 0;
	delay();
	delay();
	lcd_init();
	
loop:  
 	disdate_a(0x55,0x55);
	printf("1\n");
 	delay1();
 	disdate_a(0xff,0x00);
	
	printf("2\n");
 	delay1();
 	disdate_a(0xaa,0x55);
	
	printf("3\n");
 	delay1();
 	disdate_a(0xff,0xff);
	
	printf("4\n");
 	delay1();
 	disdate_a(0x00,0x00);
	
	printf("5\n");
 	delay1();
 	/*全屏显示字符*/
 	for(j = 0x00; j < 0x08; j++)
 	{
		i = 0x10;
		for(n = 0x00;n < 0x80; n+= 0x08)
		{ 
			cwpr(i,j,n);
			i++;
		}	
	}
	
	printf("6\n");
	disdate_a(0x00,0x00);  /* 清屏 */
	
	printf("7\n");
	/* 兆讯恒达 */
	ccwpr(0,2,24);
	ccwpr(1,2,48);
	ccwpr(2,2,72);
	ccwpr(3,2,96);
	
	printf("8\n");
	delay1();
 	delay1();
 	disdate_b(gImagedog);/*全屏画面*/
 	delay1();
 	delay1();
	
//	if (k++ < 2)
//	{
//		goto loop;
//	}
 	
}
