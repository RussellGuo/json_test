/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : mhscpu_adc.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 05/28/2017
 * Description          : ADC headfile.
 *****************************************************************************/
 
 
#ifndef __MHSCPU_ADC_H__
#define __MHSCPU_ADC_H__
 
 
#ifdef __cplusplus
extern "C" {
#endif
	
/* Include ------------------------------------------------------------------*/
#include "mhscpu.h"	
    
#define IS_ADC_PERIPH(PERIPH) ((PERIPH) == ADC0)

#define ADC_CR1_CHANNEL_MASK           ((uint32_t)0x0F)	

#define ADC_CR1_POWER_DOWN              BIT(8)    
#define ADC_CR1_IRQ_ENABLE              BIT(5)
#define ADC_SR_DONE_FLAG                BIT(0)
#define ADC_SR_FIFO_OV_FLAG             BIT(1)
#define ADC_FIFO_OV_INT_ENABLE          BIT(2)   
#define ADC_FIFO_RESET                  BIT(1)
#define ADC_FIFO_ENABLE                 BIT(0)  
#define ADC_CR2_BUFF_ENABLE             BIT(14) 
#define ADC_DIV_RESISTOR_EN_BIT         BIT(10)
    
#define ADC_DATA_OFFSET_OTPADDR                 ((MHSCPU_OTP_BASE + 0x19A))    
    
typedef enum
{
    ADC_OverFlow = 0,
    ADC_NoOverFlow = 1,
}ADC_OverFlowTypeDef;

/* ADC Channel select */  
typedef enum
{
    ADC_CHANNEL_CHARGE_VBAT =0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
	ADC_CHANNEL_7,
	ADC_CHANNEL_8,
	ADC_CHANNEL_9,
	ADC_CHANNEL_10
}ADC_ChxTypeDef;
#define IS_ADC_CHANNEL(CHANNEL_NUM) (((CHANNEL_NUM) == ADC_CHANNEL_CHARGE_VBAT) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_1) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_2) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_3) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_4) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_5) || \
									 ((CHANNEL_NUM) == ADC_CHANNEL_6) || \
									 ((CHANNEL_NUM) == ADC_CHANNEL_7) || \
									 ((CHANNEL_NUM) == ADC_CHANNEL_8) || \
									 ((CHANNEL_NUM) == ADC_CHANNEL_9) || \
                                     ((CHANNEL_NUM) == ADC_CHANNEL_10))
                                    
/* ADC Samp Select */
typedef enum 
{
   ADC_SpeedPrescaler_2    = 0x0,
   ADC_SpeedPrescaler_4    = 0x1,
   ADC_SpeedPrescaler_6    = 0x2,
   ADC_SpeedPrescaler_8    = 0x3,
}ADC_SampTypeDef;
#define IS_ADC_SAMP(SAMP)           (((SAMP) == ADC_SpeedPrescaler_2) || \
                                     ((SAMP) == ADC_SpeedPrescaler_4) || \
                                     ((SAMP) == ADC_SpeedPrescaler_6) || \
                                     ((SAMP) == ADC_SpeedPrescaler_8))
                                     

typedef struct _ADC_InitTypeDef
{
    ADC_ChxTypeDef              ADC_Channel;            /* ADC Channel select */
    ADC_SampTypeDef             ADC_SampSpeed;          /* ADC sampspeed select */
    FunctionalState             ADC_IRQ_EN;             /* ADC IRQ/Polling Select */
    FunctionalState             ADC_FIFO_EN;            /* ADC FIFO Enable Select */
} ADC_InitTypeDef;  

/* Exported constants -------------------------------------------------------*/	
/* Exported macro -----------------------------------------------------------*/	
/* Exported functions -------------------------------------------------------*/	
void ADC_Init(ADC_InitTypeDef *ADC_InitStruct);

void ADC_StartCmd(FunctionalState NewState);

void ADC_FIFODeepth(uint32_t FIFO_Deepth);

void ADC_FIFOReset(void);

void ADC_ITCmd(FunctionalState NewState);

void ADC_FIFOOverflowITcmd(FunctionalState NewState);

void ADC_BuffCmd(FunctionalState NewState);

void ADC_DivResistorCmd(FunctionalState NewState);

int32_t ADC_GetFIFOCount(void);

int32_t ADC_GetResult(void);

int32_t ADC_GetFIFOResult(uint16_t *ADCdata, uint32_t len);

uint32_t ADC_CalVoltage(uint32_t u32ADC_Value, uint32_t u32ADC_Ref_Value);

ADC_ChxTypeDef ADC_GetChannel(void);

FunctionalState ADC_IsDivResistorEnable(void);
ADC_OverFlowTypeDef ADC_IsFIFOOverflow(void);
void ADC_ChannelSwitch(ADC_ChxTypeDef ADC_Channelx);
/* Exported variables -------------------------------------------------------*/
    
/* Exported types -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif	 

#endif	/* __MHSCPU_ADC_H__ */
/************************ (C) COPYRIGHT 2015 Megahuntmicro ****END OF FILE****/
