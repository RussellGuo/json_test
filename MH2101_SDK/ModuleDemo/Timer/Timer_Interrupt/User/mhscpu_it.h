#ifndef __MHSCPU_IT_H
#define __MHSCPU_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "mhscpu.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM0_0_IRQHandler(void);
void TIM0_1_IRQHandler(void);
void TIM0_2_IRQHandler(void);
void TIM0_3_IRQHandler(void);
void TIM0_4_IRQHandler(void);
void TIM0_5_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif 
