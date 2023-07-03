/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_powerkey.h
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 25-April-2021
 * @brief               : This file provides all the SYSCTRL firmware functions
 *****************************************************************************/
 
#ifndef __MHSCPU_POWERKEY_H
#define __MHSCPU_POWEEKEY_H

#include "mhscpu.h"

typedef enum
{
    POWERKEY_ONTIME_128MS      = (uint32_t)(0),
	POWERKEY_ONTIME_500MS      = (uint32_t)(1 << 15),
	POWERKEY_ONTIME_1S         = (uint32_t)(2 << 15),
	POWERKEY_ONTIME_2S         = (uint32_t)(3 << 15),
}POWERKEY_OnTimeTypeDef;

#define IS_POWERKEY_ONTIME(TIME)                     (((TIME) == POWERKEY_ONTIME_128MS)  	|| \
                                                     ((TIME) == POWERKEY_ONTIME_500MS)  	|| \
                                                     ((TIME) == POWERKEY_ONTIME_1S)  		|| \
                                                     ((TIME) == POWERKEY_ONTIME_2S))

void POWERKEY_SoftPowerdown(FunctionalState NewState);
void POWERKEY_SetOnTime(POWERKEY_OnTimeTypeDef Time);

#endif

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/