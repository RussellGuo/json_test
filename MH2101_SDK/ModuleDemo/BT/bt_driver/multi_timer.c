#include <stdio.h>
#include <stdint.h>
#include "multi_timer.h"


Timer_t *header_timer = NULL;
volatile uint32_t _timer_ticks = 0;
volatile uint8_t period_ms = 1;

uint32_t get_timer_tick(void)
{
	return (_timer_ticks * period_ms);
}

void multi_timer_create(Timer_t *timer,uint32_t ms,timer_mode mode,void (*timer_cb)())
{
	timer->mode = mode;
	timer->timeout = ms;
	timer->timeout_cb = timer_cb;
	timer->is_occupy = 0;
	timer->next_timer = header_timer;
	header_timer = timer;
}

void multi_timer_start(Timer_t *timer)
{
	if(timer->is_occupy) return;
	timer->start_ms = get_timer_tick();
	timer->is_occupy = 1;
}

void multi_timer_restart(Timer_t *timer,uint32_t ms)
{
	timer->start_ms = get_timer_tick();
	timer->timeout = ms;
	timer->is_occupy = 1;
}


void multi_timer_stop(Timer_t *timer)
{
	timer->is_occupy  = 0;
}

void multi_timer_loop(void)
{
	Timer_t *timer;
	for(timer = header_timer;timer;timer = timer->next_timer)
	{
		if(timer->is_occupy)
		{
			if(get_timer_tick() - timer->start_ms >= timer->timeout)
			{
				if(timer->mode == TIMER_PERIOD){
					timer->start_ms = get_timer_tick();
				}else{
					multi_timer_stop(timer);
				}
				timer->timeout_cb();
			}
		}
	}
}

void multi_timer_ticks(uint8_t ms)
{
	period_ms = ms;
	_timer_ticks++;
}
