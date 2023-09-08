#ifndef _HAL_SYSTEM_H
#define _HAL_SYSTEM_H


#define CNT_PORTRECV 			8489//6791
#define CNT_PORTRECV_INT		1500//1200
#define CNT_DELAYMS				15000//12500
#define CNT_DELAYUS 			8//3

void delaycnt(unsigned int cnt);
void DelayMs(unsigned int ms);
void DelayUs(unsigned int us);
unsigned long get_system_frequency(void);
unsigned long get_ahb_frequency(void);
unsigned long get_apb_frequency(void);
#endif
