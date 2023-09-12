#ifndef _HAL_SYSTEM_H
#define _HAL_SYSTEM_H
#include <mhscpu.h>

#define CNT_PORTRECV 			8489//6791
#define CNT_PORTRECV_INT		1500//1200
#define CNT_DELAYMS				15000//12500
#define CNT_DELAYUS 			8//3

#define BIT_CLK_TIMM 		BIT21
#define BIT_TIMM_CR_EN BIT0
#define BIT_TIMM_CR_US BIT1 
#define BIT_TIMM_CR_IEDIS BIT2
#define BIT_TIMM_CR_PWM BIT3

void delaycnt(unsigned int cnt);
void DelayMs(unsigned int ms);
void DelayUs(unsigned int us);
unsigned long get_system_frequency(void);
unsigned long get_ahb_frequency(void);
unsigned long get_apb_frequency(void);
void s_PwmConfig(unsigned char ftm_id,unsigned char ch_id,unsigned long CLK,unsigned short hi,unsigned short pero);
void s_PwmOnOff(unsigned char ftm_id,unsigned char ch_id,unsigned char OnOff);
#endif
