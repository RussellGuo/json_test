/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_adc.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file provides all the ADC firmware functions
 *****************************************************************************/
 
/* Include ------------------------------------------------------------------*/
#include "mhscpu_adc.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/	
/* Private macro ------------------------------------------------------------*/	
/* Private variables --------------------------------------------------------*/	
/* Ptivate function prototypes ----------------------------------------------*/	


/******************************************************************************
* Function Name  : ADC_Init
* Description    : ��ʼ��ADC,��ʼ���ο�ֵ
* Input          : ADC_InitStruct��Ҫ��ʼ�������ݽṹָ��
* Return         : NONE
******************************************************************************/
void ADC_Init(ADC_InitTypeDef *ADC_InitStruct)
{
    assert_param(IS_ADC_CHANNEL(ADC_InitStruct->ADC_Channel));
    assert_param(IS_ADC_SAMP(ADC_InitStruct->ADC_SampSpeed));
    assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_IRQ_EN));
    assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_FIFO_EN));
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
	
    /* Select ADC Channel */
    ADC0->ADC_CR1 = (ADC0->ADC_CR1 & ~(ADC_CR1_CHANNEL_MASK)) | ADC_InitStruct->ADC_Channel; 
    /* Select ADC Channel Samping */

	ADC0->ADC_TIMER = (ADC0->ADC_TIMER & ~0xff) | (ADC_InitStruct->ADC_SampSpeed);

    /* Set ADC Interrupt */
    if (ENABLE == ADC_InitStruct->ADC_IRQ_EN )
    {
        ADC0->ADC_CR1 |= ADC_CR1_IRQ_ENABLE;
    }
	else
    {
        ADC0->ADC_CR1 &= ~ADC_CR1_IRQ_ENABLE;
    }
	
    /* Set ADC FIFO */
    if (ENABLE == ADC_InitStruct->ADC_FIFO_EN)
    {
        ADC0->ADC_FIFO |= ADC_FIFO_ENABLE;
    }
	else
    {
        ADC0->ADC_FIFO &= ~ADC_FIFO_ENABLE;
    }
}

/******************************************************************************
* Function Name  : ADC_GetChannel
* Description    : ��ȡADC��ǰʹ�õ�ͨ��
* Input          : None
* Return         : ADC_ChxTypeDef
******************************************************************************/
ADC_ChxTypeDef ADC_GetChannel(void)
{
    return (ADC_ChxTypeDef)(ADC0->ADC_CR1 & ADC_CR1_CHANNEL_MASK);
}

/******************************************************************************
* Function Name  : ADC_GetResult
* Description    : ������ȡADC��Ӧͨ����ֵ,�г�ʱ���
* Input          : None
* Return         : 0:��ȡ��ʱ  Other:ADCֵ
******************************************************************************/
int32_t ADC_GetResult(void)
{
    while((!(ADC0->ADC_SR & ADC_SR_DONE_FLAG)));

    return (ADC0->ADC_DATA & 0xFFF) - (*(int16_t *)ADC_DATA_OFFSET_OTPADDR);
}

/******************************************************************************
* Function Name  : ADC_GetFIFOResult
* Description    : ������ȡADC��Ӧͨ����ֵ,�г�ʱ���
* Input          : ADCdata:FIFOָ�� 
                   len:����
* Return         : FIFO��ADC����
******************************************************************************/
int32_t ADC_GetFIFOResult(uint16_t *ADCdata, uint32_t len)
{
    uint32_t i, adccount = 0;
	
    //��ȡ���ȴ���FIFO���޸���
    if (NULL == ADCdata)
    {
        return 0;
    }

    adccount = ADC_GetFIFOCount();
    if (adccount > len)
    {
        adccount = len;
    }
    
    for (i = 0; i < adccount; i++)
    {
        ADCdata[i] = (ADC0->ADC_DATA & 0xFFF) - (*(int16_t *)ADC_DATA_OFFSET_OTPADDR);
    }
	
    return adccount;
}

/******************************************************************************
* Function Name  : ADC_GetFIFOCount
* Description    : ��ȡFIFO�����ݵĸ���
* Input          : None
* Return         : 0:δ����FIFOģʽ other:FIFO�����ݵĸ���
******************************************************************************/
int32_t ADC_GetFIFOCount(void)
{
    if ((ADC0->ADC_FIFO & ADC_FIFO_ENABLE) != ADC_FIFO_ENABLE)
    {
        return 0;
    }
	
    return ADC0->ADC_FIFO_FL & 0x1F;
}

/******************************************************************************
* Function Name  : ADC_FifoReset
* Description    : ����ADC fifo
* Input          : None
* Return         : None
******************************************************************************/
void ADC_FIFOReset(void)
{
    ADC0->ADC_FIFO |= ADC_FIFO_RESET;
}

/******************************************************************************
* Function Name  : ADC_ITCmd
* Description    : ����ADC�жϵĿ���
* Input          : NewState��ADC�жϿ��ز���������ȡENABLE��DISABLE
* Return         : NONE
******************************************************************************/
void ADC_ITCmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));
	
    if (DISABLE != NewState)
	{
		ADC0->ADC_CR1 |= ADC_CR1_IRQ_ENABLE;
	}
	else
	{
		ADC0->ADC_CR1 &= ~ADC_CR1_IRQ_ENABLE;
	}
}

/******************************************************************************
* Function Name  : ADC_FIFOOverflowITcmd
* Description    : ����ADC FIFO����жϵĿ���
* Input          : NewState��ENABLE/DISABLE
* Return         : NONE
******************************************************************************/
void ADC_FIFOOverflowITcmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));
	
    if (DISABLE != NewState)
	{
		ADC0->ADC_FIFO |= ADC_FIFO_OV_INT_ENABLE;
	}
	else
	{
		ADC0->ADC_FIFO &= ~ADC_FIFO_OV_INT_ENABLE;
    }
}

/******************************************************************************
* Function Name  : ADC_IsFIFOOverflow
* Description    : ��ȡADC����ж��Ƿ���λ
* Input          : NONE
* Return         : ADC_OverFlow/ADC_NoOverFlow
******************************************************************************/
ADC_OverFlowTypeDef ADC_IsFIFOOverflow(void)
{
    if((ADC0->ADC_SR & ADC_SR_FIFO_OV_FLAG) == ADC_SR_FIFO_OV_FLAG)
    {
        return ADC_OverFlow;
    }
    else
    {
        return ADC_NoOverFlow;
    }
}

/******************************************************************************
* Function Name  : ADC_StartCmd
* Description    : ADCת����������
* Input          : NewState��ADCת���������ز���������ȡENABLE��DISABLE
* Return         : NONE
******************************************************************************/
void ADC_StartCmd(FunctionalState NewState)
{
    if (DISABLE != NewState)
    {
        ADC0->ADC_CR1 &= ~ADC_CR1_POWER_DOWN;
    }
    else
    {
        ADC0->ADC_CR1 |= ADC_CR1_POWER_DOWN;
    }
}

/******************************************************************************
* Function Name  : ADC_FIFODeepth
* Description    : ADC fifo �������
* Input          : FIFO_Deepth ����fifo��ȣ����ֵ0x20
* Return         : NONE
******************************************************************************/
void ADC_FIFODeepth(uint32_t FIFO_Deepth)
{
    ADC0->ADC_FIFO_THR = FIFO_Deepth - 1;
}

/******************************************************************************
* Function Name  : ADC_CalVoltage
* Description    : ����ת����ĵ�ѹֵ
* Input          : u32ADC_Value: ADC�ɼ�ֵ
*                : u32ADC_Ref_Value: ADC�ο���ѹ
* Return         : ����ת����ĵ�ѹֵ
******************************************************************************/
uint32_t ADC_CalVoltage(uint32_t u32ADC_Value, uint32_t u32ADC_Ref_Value)
{
    return (u32ADC_Value * u32ADC_Ref_Value / 4095);
}

/******************************************************************************
* Function Name  : ADC_BuffCmd
* Description    : ADC Buffʹ�ܿ���
* Input          : NewState:ADC BUFFʹ�ܿ���,����ȡENABLE��DISABLE
* Return         : NONE
******************************************************************************/
void ADC_BuffCmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));
	
    if (DISABLE != NewState)
	{
		ADC0->ADC_CR2 &= ~ADC_CR2_BUFF_ENABLE;
	}
	else
	{
		ADC0->ADC_CR2 |= ADC_CR2_BUFF_ENABLE;
	}
}

/******************************************************************************
* Function Name  : ADC_DivResistorCmd
* Description    : ADC �ڲ���ѹ����ʹ�ܿ���
* Input          : NewState:ADC BUFFʹ�ܿ���,����ȡENABLE��DISABLE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void ADC_DivResistorCmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if(DISABLE != NewState)
	{
		ADC0->ADC_CR2 |= ADC_DIV_RESISTOR_EN_BIT;
	}
	else
	{
		ADC0->ADC_CR2 &= ~ADC_DIV_RESISTOR_EN_BIT;
	}
}

/******************************************************************************
* Function Name  : ADC_IsDivResistorEnable
* Description    : ��ȡADC �ڲ���ѹ����ʹ��״̬
* Input          : NONE
* Return         : FunctionalState
******************************************************************************/
FunctionalState ADC_IsDivResistorEnable(void)
{
    if (ADC0->ADC_CR2 & ADC_DIV_RESISTOR_EN_BIT)
    {
        return ENABLE;
    }
    else
    {
        return DISABLE;
    }
}
/******************************************************************************
* Function Name  : ADC_ChannelSwitch
* Description    : ADC �л�ͨ��
* Input          : ADC_Channelx: ADCͨ��
* Output         : NONE
* Return         : NONE
* Channel_1		 : PC0
* Channel_2		 : PC1
* Channel_3		 : PC2
* Channel_4		 : PC3
* Channel_5		 : PC4
* Channel_6		 : PC5
* Channel_7		 : PE13
* Channel_8		 : PE14
* Channel_9		 : PE15
* Channel_10	 : PD0
******************************************************************************/

void ADC_ChannelSwitch(ADC_ChxTypeDef ADC_Channelx)
{
	uint8_t i = 7;
		
	assert_param(IS_ADC_CHANNEL(ADC_Channelx));

	ADC0->ADC_CR1 = (ADC0->ADC_CR1 & ~(0xf)) | ADC_Channelx; 
	
	while(i-- > 0)
	{
		ADC_GetResult();
	}

}

/************************ (C) COPYRIGHT 2017 Megahuntmicro ****END OF FILE****/
