#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "hw_config.h"
#include "multi_timer.h"

volatile uint32_t sys_tick = 0;

void TIM0_0_IRQHandler(void)
{
  sys_tick++;
	multi_timer_ticks(10);
  TIM_ClearITPendingBit(TIMM0, TIM_0);
}

uint32_t get_tick_count(void)
{
	return sys_tick * 10;
}

uint32_t get_diff_tick(uint32_t last_time)
{
	uint32_t now_tick = get_tick_count();
	if(now_tick >= last_time)
	{
		return (now_tick - last_time);
	}
	else
	{
		return (now_tick + (~last_time));
	}
}

void m_delay(uint32_t ms)
{
	uint32_t time = get_tick_count();
	while(get_diff_tick(time) < ms);
}


