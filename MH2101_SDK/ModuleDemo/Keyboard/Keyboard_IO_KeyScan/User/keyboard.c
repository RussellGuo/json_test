#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "keyboard.h"

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t Pin;
}KEYBOARD_PIN_T;

static KEYBOARD_PIN_T Keyboard_Column_Table[] = 
{
    {Keyboard_Column1_Port, Keyboard_Column_1},
    {Keyboard_Column2_Port, Keyboard_Column_2},
    {Keyboard_Column3_Port, Keyboard_Column_3},
};

Keyboard_Param_T Keyboard_Param;

static void Keyboard_Delay(uint32_t nCount);
static void EXTI_ClearITBits(uint32_t EXTI_Line, uint32_t EXTI_PinSource);

void Keyboard_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	
    memset(&Keyboard_Param, 0x00, sizeof(Keyboard_Param));
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = Keyboard_Row_All;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(Keyboard_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = Keyboard_Column_1;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(Keyboard_Column1_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = Keyboard_Column_2;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(Keyboard_Column2_Port, &GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = Keyboard_Column_3;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(Keyboard_Column3_Port, &GPIO_InitStructure);    
    Keyboard_Column_Low();
   
    EXTI_DeInit();
    EXTI_ClearITPendingBit(Keyboard_EXTI);
    EXTI_LineConfig(Keyboard_EXTI, Keyboard_EXTI_Source, EXTI_Trigger_Falling);
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);    
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Keyboard_IRQHandler(void)
{
    uint8_t i = 0;    
    uint32_t intFlag = 0;

    intFlag = EXTI_GetITLineStatus(Keyboard_EXTI) & Keyboard_EXTI_Source;
    switch (intFlag)
    {
        case Keyboard_EXTI_Source_1:
            Keyboard_Column_High();
            for (i = 0; i < sizeof(Keyboard_Column_Table)/sizeof(Keyboard_Column_Table[0]); i++)
            {
                GPIO_ResetBits(Keyboard_Column_Table[i].GPIOx, Keyboard_Column_Table[i].Pin);
                if (!GPIO_ReadInputDataBit(Keyboard_Port, Keyboard_Row_1))
                {     
                    Keyboard_Delay(DELAY_COUNT);
                    if (!GPIO_ReadInputDataBit(Keyboard_Port, Keyboard_Row_1))
                    {
                        Keyboard_Param.keyCode = i;
                        Keyboard_Param.keyPressFlag = 1;
                        break;
                     }
                }  
            }             
            Keyboard_Column_Low(); 
            EXTI_ClearITBits(Keyboard_EXTI, Keyboard_EXTI_Source_1);
            break;

        case Keyboard_EXTI_Source_2:
            Keyboard_Column_High();
            for (i = 0; i < sizeof(Keyboard_Column_Table)/sizeof(Keyboard_Column_Table[0]); i++)
            {
                GPIO_ResetBits(Keyboard_Column_Table[i].GPIOx, Keyboard_Column_Table[i].Pin);
                if (!GPIO_ReadInputDataBit(Keyboard_Port, Keyboard_Row_2))
                {     
                    Keyboard_Delay(DELAY_COUNT);
                    if (!GPIO_ReadInputDataBit(Keyboard_Port, Keyboard_Row_2))
                    {
                        Keyboard_Param.keyCode = 4+i;
                        Keyboard_Param.keyPressFlag = 1;
                        break;            
                     }
                }  
            }
            Keyboard_Column_Low();            
            EXTI_ClearITBits(Keyboard_EXTI, Keyboard_EXTI_Source_2);
            break;    
            
        default:
            break;
    }
 
    NVIC_ClearPendingIRQ(Keyboard_IRQn);
}

void SysTick_Handler(void)
{
    if (Keyboard_Param.keyPressFlag)
    {
        Keyboard_Param.keyPressFlag = 0;
        Keyboard_Param.callbackFunc(Keyboard_Param.keyCode);
    }
}

void Keyboard_CallbackRegister(Keyboard_CallbackFunc p_func)
{
    Keyboard_Param.callbackFunc = p_func;
}

static void Keyboard_Delay(uint32_t nCount)
{
    for (; nCount > 0; nCount--);
}

static void EXTI_ClearITBits(uint32_t EXTI_Line, uint32_t EXTI_PinSource)
{
	GPIO->INTP_TYPE_STA[EXTI_Line].INTP_STA |= EXTI_PinSource;
}
