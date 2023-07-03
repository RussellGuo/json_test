/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_charge.h
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 25-April-2021
 *****************************************************************************/

#ifndef __MHSCPU_CHARGE_H
#define __MHSCPU_CHARGE_H

#include "mhscpu.h"

typedef enum
{
    SYSCTRL_CHG_CURRENT_50MA       = (uint32_t)0x00,
    SYSCTRL_CHG_CURRENT_100MA      = (uint32_t)0x01,
    SYSCTRL_CHG_CURRENT_150MA      = (uint32_t)0x02,
    SYSCTRL_CHG_CURRENT_200MA      = (uint32_t)0x03,
    SYSCTRL_CHG_CURRENT_240MA      = (uint32_t)0x04,
    SYSCTRL_CHG_CURRENT_290MA      = (uint32_t)0x05,
    SYSCTRL_CHG_CURRENT_330MA      = (uint32_t)0x06,
    SYSCTRL_CHG_CURRENT_370MA      = (uint32_t)0x07,
}CHARGE_CurrentTypeDef;

#define IS_CHARGE_CURRENT(CUR)                      (((CUR) == SYSCTRL_CHG_CURRENT_50MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_100MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_150MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_200MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_240MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_290MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_330MA)  || \
                                                     ((CUR) == SYSCTRL_CHG_CURRENT_370MA))

void CHARGE_Cmd(FunctionalState NewState);
void CHARGE_SetCurrent(CHARGE_CurrentTypeDef ChargeCurrent);
uint8_t CHARGE_GetState(void);


#endif



/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/



























