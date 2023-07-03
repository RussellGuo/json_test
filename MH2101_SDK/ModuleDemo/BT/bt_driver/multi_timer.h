#ifndef __MULTI_TIMER_H
#define __MULTI_TIMER_H

#include <stdint.h>


typedef enum{
	TIMER_SINGLE_SHOT,
	TIMER_PERIOD
}timer_mode;

typedef struct timer
{
	uint8_t is_occupy;//�Ƿ�ռ�ö�ʱ
	uint8_t mode;//��ʱģʽ
	uint32_t	timeout;//��ʱʱ��
	uint32_t start_ms;//��ʼ��ʱ
	void (*timeout_cb)(void);
	struct timer *next_timer;
}Timer_t;

void multi_timer_create(Timer_t *timer,uint32_t ms,timer_mode mode,void (*timer_cb)());
void multi_timer_start(Timer_t *timer);
void multi_timer_restart(Timer_t *timer,uint32_t ms);
void multi_timer_stop(Timer_t *timer);
void multi_timer_loop(void);
void multi_timer_ticks(uint8_t ms);

#endif
