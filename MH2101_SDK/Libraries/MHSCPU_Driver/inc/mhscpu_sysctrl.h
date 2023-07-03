/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_sysctrl.h
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file contains all the functions prototypes for the SYSCTRL firmware library
 *****************************************************************************/
 
#ifndef __MHSCPU_SYSCTRL_H
#define __MHSCPU_SYSCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mhscpu.h"

#define SYSCTRL_FREQ_SEL_POWERMODE_Pos                  (24)
#define SYSCTRL_FREQ_SEL_POWERMODE_Mask                 (0x07 << SYSCTRL_FREQ_SEL_POWERMODE_Pos)     
#define SYSCTRL_FREQ_SEL_POWERMODE_CPUOFF               (0x00 << SYSCTRL_FREQ_SEL_POWERMODE_Pos) 
#define SYSCTRL_FREQ_SEL_POWERMODE_SLEEP                (0x01 << SYSCTRL_FREQ_SEL_POWERMODE_Pos) 
#define SYSCTRL_FREQ_SEL_POWERMODE_POWERDOWN            (0x03 << SYSCTRL_FREQ_SEL_POWERMODE_Pos) 


/** @defgroup SYSCTRL_Exported_Types
  * @{
  */
typedef struct
{
    uint32_t PLL_Frequency;     /*!< returns PLL frequency expressed in Hz */
    uint32_t CPU_Frequency;     /*!< returns CPU frequency expressed in Hz */
    uint32_t HCLK_Frequency;    /*!< returns HCLK frequency expressed in Hz */
    uint32_t PCLK_Frequency;    /*!< returns PCLK frequency expressed in Hz */
}SYSCTRL_ClocksTypeDef;

typedef enum
{
    SleepMode_CpuOff = 0,       /*!< CPU Sleep  */
    SleepMode_Sleep,            /*!< Sleep      */
    SleepMode_PowerDown,        /*!< PowerDown  */
    SleepMode_Invalid
}SleepMode_TypeDef;
#define IS_ALL_SLEEP_MODE(MODE)                     ((MODE) < SleepMode_Invalid) 

typedef enum 
{
    SELECT_EXT12M,
    SELECT_INC12M,
	SELECT_EXTPA11,
	SELECT_EXTPB7
} SYSCLK_SOURCE_TypeDef;
#define IS_SYSCLK_SOURCE(FREQ)                      (((FREQ) == SELECT_EXT12M) || \
                                                    ((FREQ) == SELECT_INC12M)  || \
													((FREQ) == SELECT_EXTPA11) || \
													((FREQ) == SELECT_EXTPB7))
typedef enum
{
	SYSCTRL_WakeMode_GPIO_Now			= (uint32_t)(0),
	SYSCTRL_WakeMode_GPIO_AfterGlitch 	= (uint32_t)(1),
	SYSCTRL_WakeMode_KBD				= (uint32_t)(1 << 11),
	SYSCTRL_WakeMode_RTC				= (uint32_t)(1 << 12),
	SYSCTRL_WakeMode_MSR				= (uint32_t)(1 << 13),
	SYSCTRL_WakeMode_SENSOR				= (uint32_t)(1 << 14),
	SYSCTRL_WakeMode_AWD				= (uint32_t)(1 << 15),
	SYSCTRL_WakeMode_CHG				= (uint32_t)(1 << 16)
}SYSCTRL_WakeModeTypeDef;

#define IS_SYSCTRL_WAKE_MODE(MODE)		(((MODE) == SYSCTRL_WakeMode_GPIO_Now) 			|| \
										((MODE) == SYSCTRL_WakeMode_GPIO_AfterGlitch)	|| \
										((MODE) == SYSCTRL_WakeMode_KBD)				|| \
										((MODE) == SYSCTRL_WakeMode_RTC)				|| \
										((MODE) == SYSCTRL_WakeMode_MSR)				|| \
										((MODE) == SYSCTRL_WakeMode_SENSOR)				|| \
										((MODE) == SYSCTRL_WakeMode_CHG))


typedef enum
{
	SYSCTRL_PDWakeMode_PF6				= (uint32_t)(1 << 8),
	SYSCTRL_PDWakeMode_PA15 			= (uint32_t)(1 << 9),
	SYSCTRL_PDWakeMode_PA14 			= (uint32_t)(1 << 10),
	SYSCTRL_PDWakeMode_SENSOR			= (uint32_t)(1 << 12),
	SYSCTRL_PDWakeMode_ALL			    = (uint32_t)(0x17 << 8),	
}SYSCTRL_PDWakeModeTypeDef;

#define IS_SYSCTRL_PDWAKE_MODE(MODE)		(((MODE) == SYSCTRL_PDWakeMode_PF6) 	|| \
											 ((MODE) == SYSCTRL_PDWakeMode_PA15)	|| \
											 ((MODE) == SYSCTRL_PDWakeMode_PA14)	|| \
											 ((MODE) == SYSCTRL_PDWakeMode_SENSOR)  || \
											 ((MODE) == SYSCTRL_PDWakeMode_ALL))

typedef enum
{
	SYSCTRL_CALIBRATE_12M_USE_USB = 0,
	SYSCTRL_CALIBRATE_12M_USE_XTA
}SYSCTRL_CalibrateTypeDef;

#define IS_SYSCTRL_CALIBRATE_MODE(MODE)	(((MODE) == SYSCTRL_CALIBRATE_12M_USE_USB) 		|| \
										((MODE) == SYSCTRL_CALIBRATE_12M_USE_XTA))


typedef enum
{
    SYSCTRL_PLL_108MHz      = (uint32_t)0x01,
    SYSCTRL_PLL_120MHz      = (uint32_t)0x02,
    SYSCTRL_PLL_132MHz      = (uint32_t)0x03,
    SYSCTRL_PLL_144MHz      = (uint32_t)0x04,
    SYSCTRL_PLL_156MHz      = (uint32_t)0x05,
    SYSCTRL_PLL_168MHz      = (uint32_t)0x06,
    SYSCTRL_PLL_180MHz      = (uint32_t)0x07,
    SYSCTRL_PLL_192MHz      = (uint32_t)0x08,
    SYSCTRL_PLL_204MHz      = (uint32_t)0x09
}SYSCTRL_PLL_TypeDef;
#define IS_PLL_FREQ(FREQ)                          (((FREQ) == SYSCTRL_PLL_108MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_120MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_132MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_144MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_156MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_168MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_180MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_192MHz)  || \
                                                    ((FREQ) == SYSCTRL_PLL_204MHz))
/**
  * @}
  */

#define SYSCTRL_PLL_Div_None                       ((uint32_t)0x00)
#define SYSCTRL_PLL_Div2                           ((uint32_t)0x01)
#define SYSCTRL_PLL_Div4                           ((uint32_t)0x10)
#define IS_GET_SYSCTRL_PLL_DIV(DIV)                (((DIV) == SYSCTRL_PLL_Div_None) || \
                                                    ((DIV) == SYSCTRL_PLL_Div2) || \
													((DIV) == SYSCTRL_PLL_Div4))

#define SYSCTRL_HCLK_Div_None                      ((uint32_t)0x00)
#define SYSCTRL_HCLK_Div2                          ((uint32_t)0x01)
#define IS_GET_SYSCTRL_HCLK_DIV(DIV)               (((DIV) == SYSCTRL_HCLK_Div_None) || \
                                                    ((DIV) == SYSCTRL_HCLK_Div2)) 

#define SYSCTRL_PCLK_Div2                          ((uint32_t)0x01)
#define SYSCTRL_PCLK_Div4                          ((uint32_t)0x02)
#define IS_GET_SYSCTRL_PCLK_DIV(DIV)               (((DIV) == SYSCTRL_PCLK_Div2) || \
                                                    ((DIV) == SYSCTRL_PCLK_Div4)) 
      
#define SYSCTRL_CHIP_SN_ADDR                        ((uint32_t)0x4000800A)
#define SYSCTRL_CHIP_SN_LEN                         ((uint32_t)0x10)

#define SYSCTRL_USB_DOWNLOAD_FLAG                    ((uint32_t)0X55)

#define IS_RST_FLAG(FLAG)                          ((((FLAG) & ~SYSCTRL_RST_FLAG_ALL) == 0x00) && ((FLAG) != (uint32_t)0x00))


void SYSCTRL_AHBPeriphClockCmd(uint32_t SYSCTRL_AHBPeriph, FunctionalState NewState);
void SYSCTRL_AHBPeriphResetCmd(uint32_t SYSCTRL_AHBPeriph, FunctionalState NewState);
void SYSCTRL_APBPeriphClockCmd(uint32_t SYSCTRL_APBPeriph, FunctionalState NewState);
void SYSCTRL_APBPeriphResetCmd(uint32_t SYSCTRL_APBPeriph, FunctionalState NewState);

void SYSCTRL_EnterSleep(SleepMode_TypeDef SleepMode);

void SYSCTRL_SYSCLKSourceSelect(SYSCLK_SOURCE_TypeDef SourceSelect);
void SYSCTRL_PLLConfig(SYSCTRL_PLL_TypeDef PLL_Freq);
void SYSCTRL_PLLDivConfig(uint32_t PLL_Div);
void SYSCTRL_HCLKConfig(uint32_t HCLK_Div);
void SYSCTRL_PCLKConfig(uint32_t PCLK_Div);

void SYSCTRL_GetClocksFreq(SYSCTRL_ClocksTypeDef* SYSCTRL_Clocks);
uint8_t SYSCTRL_GetResetFlag(void);
void SYSCTRL_ClearResetFlag(uint32_t ResetFlag);

void SYSCRTRL_WakeUpCmd(SYSCTRL_WakeModeTypeDef WakeMode, FunctionalState NewState);
void SYSCRTRL_PDWakeUpCmd(SYSCTRL_PDWakeModeTypeDef WakeMode, FunctionalState NewState);

void SYSCTRL_Calibrate12M(SYSCTRL_CalibrateTypeDef CalibrateMode);
uint32_t SYSCRTRL_GetChipID(void);

void SYSCTRL_GetChipSN(unsigned char *ChipSN);

void SYSCTRL_SoftReset(void);
void SYSCTRL_EnterUSBDownload(void);

#ifdef __cplusplus
}
#endif

#endif 

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/
