#ifndef __TIMER_H
#define	__TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif 

#include "mhscpu.h"

extern volatile uint32_t tick_Timer;    
    
void TIMER_Configuration(void);    

void Delay(uint32_t Delay);

    
#ifdef __cplusplus
}
#endif
	
#endif   ///< __TIMER_H

