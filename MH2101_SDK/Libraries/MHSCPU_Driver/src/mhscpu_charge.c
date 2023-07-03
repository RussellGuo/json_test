/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_charge.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 25-April-2021
 * @brief               : This file provides all the SYSCTRL firmware functions
 *****************************************************************************/


#include "mhscpu_charge.h"

/**
 * @brief  Enable or disables the charger
 * @param  NewState: ENABLE or DISABLE
 *        
 * @retval None
 */
void CHARGE_Cmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
	{
        SYSCTRL->CHG_CSR |= BIT(11);
    }
    else
	{
        SYSCTRL->CHG_CSR &= ~BIT(11);
    }
	
	SYSCTRL->CHG_CSR |= BIT(20);
	SYSCTRL->CHG_CSR &= ~BIT(20);	
}


/** @brief  Set Charge Current
 * @param  ChargeCurrent Charge current set value.
 * @retval None
 */
void CHARGE_SetCurrent(CHARGE_CurrentTypeDef ChargeCurrent)
{
    assert_param(IS_CHARGE_CURRENT(ChargeCurrent));

    switch ((uint32_t)ChargeCurrent)
	{
        case SYSCTRL_CHG_CURRENT_50MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_50MA);
            break;

        case SYSCTRL_CHG_CURRENT_100MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_100MA);
            break;

        case SYSCTRL_CHG_CURRENT_150MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_150MA);
            break;

        case SYSCTRL_CHG_CURRENT_200MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_200MA);
            break;

        case SYSCTRL_CHG_CURRENT_240MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_240MA);
            break;

        case SYSCTRL_CHG_CURRENT_290MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_290MA);
            break;

        case SYSCTRL_CHG_CURRENT_330MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_330MA);
            break;

        case SYSCTRL_CHG_CURRENT_370MA:
            SYSCTRL->CHG_CSR = ((SYSCTRL->CHG_CSR & (~SYSCTRL_CHG_CSR_CURRENT_SEL_Mask)) | SYSCTRL_CHG_CSR_CURRENT_SEL_370MA);
            break;
    }
	
	SYSCTRL->CHG_CSR |= BIT(20);
	SYSCTRL->CHG_CSR &= ~BIT(20);		
}

/** @brief  Get Charge State
 * @param  None
 * @retval Return Charge State
 */
uint8_t CHARGE_GetState(void)
{
    return (SYSCTRL->CHG_CSR & SYSCTRL_CHG_CSR_STATE_Mask) >> SYSCTRL_CHG_CSR_STATE_Pos;
}

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/

