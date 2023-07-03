/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_powerkey.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 25-April-2021
 * @brief               : This file provides all the SYSCTRL firmware functions
 *****************************************************************************/

#include "mhscpu_powerkey.h"


void POWERKEY_SoftPowerdown(FunctionalState NewState)
{
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	
	if (DISABLE != NewState)
	{
		SENSOR->SEN_ANA0 |= BIT(31);
	}
	else
	{
		SENSOR->SEN_ANA0 &= ~BIT(31);
	}

}


void POWERKEY_SetOnTime(POWERKEY_OnTimeTypeDef Time)
{
	assert_param(IS_POWERKEY_ONTIME(Time));
	
	SENSOR->SEN_ANA1 = (SENSOR->SEN_ANA1 & ~(0x03 << 15)) | Time;
}

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/

