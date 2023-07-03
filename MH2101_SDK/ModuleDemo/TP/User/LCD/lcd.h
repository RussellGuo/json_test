#ifndef __LCD_H
#define __LCD_H

#include "mhscpu.h"

void TftLCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);//ָ�����������ɫ
void TftLCD_ClearDma(uint16_t color);

void TftLCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);//��ָ��λ�û�һ����
void TftLCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);//��ָ��λ�û�һ����
void TftLCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);//��ָ��λ�û�һ������
void TftLCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);//��ָ��λ�û�һ��Բ

void TftLCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);//��ʾһ���ַ�
void TftLCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);//��ʾ�ַ���

void TftLCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);//��ʾ��������
void TftLCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);//��ʾ��λС������

void TftLCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]);//��ʾͼƬ


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0xF81F
#define GRED 			 0xFFE0
#define GBLUE			 0x07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0xBC40 //��ɫ
#define BRRED 			 0xFC07 //�غ�ɫ
#define GRAY  			 0x8430 //��ɫ
#define DARKBLUE      	 0x01CF	//����ɫ
#define LIGHTBLUE      	 0x7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0x5458 //����ɫ
#define LIGHTGREEN     	 0x841F //ǳ��ɫ
#define LGRAY 			 0xC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0xA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0x2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


#endif
