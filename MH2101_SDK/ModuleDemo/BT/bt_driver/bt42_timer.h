#ifndef __BT42_TIMER_H
#define __BT42_TIMER_H



uint32_t get_tick_count(void);
void m_delay(uint32_t ms);
uint32_t get_diff_tick(uint32_t last_time);


#endif

