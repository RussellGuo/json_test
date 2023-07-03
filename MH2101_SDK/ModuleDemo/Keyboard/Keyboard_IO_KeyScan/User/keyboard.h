#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#ifdef __cplusplus
extern "C" 
{
#endif 

/* Includes ------------------------------------------------------------------*/
#include "mhscpu.h"

#define Keyboard_Port               GPIOC 
#define Keyboard_Row_1              GPIO_Pin_14
#define Keyboard_Row_2              GPIO_Pin_12
#define Keyboard_Row_All            (Keyboard_Row_1 | Keyboard_Row_2) 

    
#define Keyboard_Column1_Port       GPIOC     
#define Keyboard_Column2_Port       GPIOD    
#define Keyboard_Column3_Port       GPIOD   
    
#define Keyboard_Column_1           GPIO_Pin_0   
#define Keyboard_Column_2           GPIO_Pin_12   
#define Keyboard_Column_3           GPIO_Pin_13  
#define Keyboard_Column_All         (Keyboard_Column_1 | Keyboard_Column_2 | Keyboard_Column_3)   
    
#define Keyboard_Column_High() \
    do \
    { \
       GPIO_SetBits(Keyboard_Column1_Port, Keyboard_Column_1); \
       GPIO_SetBits(Keyboard_Column2_Port, Keyboard_Column_2); \
       GPIO_SetBits(Keyboard_Column3_Port, Keyboard_Column_3); \
    }while(0)
    
#define Keyboard_Column_Low() \
    do \
    { \
       GPIO_ResetBits(Keyboard_Column1_Port, Keyboard_Column_1); \
       GPIO_ResetBits(Keyboard_Column2_Port, Keyboard_Column_2); \
       GPIO_ResetBits(Keyboard_Column3_Port, Keyboard_Column_3); \
    }while(0)
        
#define Keyboard_EXTI               EXTI_Line2 
#define Keyboard_EXTI_Source_1      EXTI_PinSource14
#define Keyboard_EXTI_Source_2      EXTI_PinSource12
#define Keyboard_EXTI_Source        (Keyboard_EXTI_Source_1 | Keyboard_EXTI_Source_2)

#define Keyboard_IRQn               EXTI2_IRQn    
#define Keyboard_IRQHandler         EXTI2_IRQHandler

#define DELAY_COUNT                 0xFFFF    

typedef void (*Keyboard_CallbackFunc)(uint8_t keyVal);    
    
typedef struct Keyboard_Param_S
{
    uint8_t keyCode;
    uint8_t keyPressFlag;
    Keyboard_CallbackFunc callbackFunc;  
}Keyboard_Param_T;
    
void Keyboard_Init(void);
void Keyboard_IRQHandler(void);
 
void Keyboard_CallbackRegister(Keyboard_CallbackFunc p_func);


#ifdef __cplusplus
}
#endif

#endif   ///< __KEYBOARD_H_


