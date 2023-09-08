#include "mhscpu.h"
#include "hal_gpio_int.h"
#include "string.h"
#include "stdio.h"
#define NORMAL_IO_PRIORITY      6 // 默认的优先级
#define ICC_IO_PRIORITY         1 // IC模块优先级
#define RF_INT_IO_PRIORITY      1 // NFC模块优先级

#define MAX_GPIO_PER_GROUP 16 // 每组GPIO中的GPIO个数

static callback_isr_cb GPIO_A_Isr[MAX_GPIO_PER_GROUP];
static callback_isr_cb GPIO_B_Isr[MAX_GPIO_PER_GROUP];
static callback_isr_cb GPIO_C_Isr[MAX_GPIO_PER_GROUP];
static callback_isr_cb GPIO_D_Isr[MAX_GPIO_PER_GROUP];
static callback_isr_cb GPIO_E_Isr[MAX_GPIO_PER_GROUP];
static callback_isr_cb TIM0_Isr;
static callback_isr_cb TIM1_Isr;
static callback_isr_cb TIM2_Isr;
static callback_isr_cb TIM3_Isr;
static callback_isr_cb TIM4_Isr;
static callback_isr_cb TIM5_Isr;
static callback_isr_cb TIM6_Isr;
static callback_isr_cb TIM7_Isr;

void EXTI0_IRQHandler(void) // GPIOA_ISR
{
    int id = 0;
    int group = GPIO_GROUP_A;
    unsigned int intsta = GPIO->INTP_TYPE_STA[group].INTP_STA;
    for (id = 0; id < MAX_GPIO_PER_GROUP; id++) {
        if ((intsta & (1<<id)) && GPIO_A_Isr[id]) {
            GPIO_A_Isr[id]();
        }
    }
    GPIO->INTP_TYPE_STA[group].INTP_STA = intsta;
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler(void) // GPIOB_ISR
{
    int id = 0;
    int group = GPIO_GROUP_B;
    unsigned int intsta = GPIO->INTP_TYPE_STA[group].INTP_STA;
    for (id = 0; id < MAX_GPIO_PER_GROUP; id++) {
        if ((intsta & (1<<id)) && GPIO_B_Isr[id]) {
            GPIO_B_Isr[id]();
        }
    }
    GPIO->INTP_TYPE_STA[group].INTP_STA = intsta;
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

void EXTI2_IRQHandler(void) // GPIOC_ISR
{
    int id = 0;
    int group = GPIO_GROUP_C;
    unsigned int intsta = GPIO->INTP_TYPE_STA[group].INTP_STA;
    for (id = 0; id < MAX_GPIO_PER_GROUP; id++) {
        if ((intsta & (1<<id)) && GPIO_C_Isr[id]) {
            GPIO_C_Isr[id]();
        }
    }
    GPIO->INTP_TYPE_STA[group].INTP_STA = intsta;
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void) // GPIOD_ISR
{
    int id = 0;
    int group = GPIO_GROUP_D;
    unsigned int intsta = GPIO->INTP_TYPE_STA[group].INTP_STA;
    for (id = 0; id < MAX_GPIO_PER_GROUP; id++) {
        if ((intsta & (1<<id)) && GPIO_D_Isr[id]) {
            GPIO_D_Isr[id]();
        }
    }
    GPIO->INTP_TYPE_STA[group].INTP_STA = intsta;
    NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

void EXTI4_IRQHandler(void) // GPIOD_ISR
{
    int id = 0;
    int group = GPIO_GROUP_E;
    unsigned int intsta = GPIO->INTP_TYPE_STA[group].INTP_STA;
    for (id = 0; id < MAX_GPIO_PER_GROUP; id++) {
        if ((intsta & (1<<id)) && GPIO_E_Isr[id]) {
            GPIO_E_Isr[id]();
        }
    }
    GPIO->INTP_TYPE_STA[group].INTP_STA = intsta;
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

void TIM0_0_IRQHandler(void)
{
    if(TIM0_Isr)
    TIM0_Isr();
    NVIC_ClearPendingIRQ(TIM0_0_IRQn);
}

void TIM0_1_IRQHandler(void)
{
    if(TIM1_Isr)
    TIM1_Isr();
    NVIC_ClearPendingIRQ(TIM0_1_IRQn);

}

void TIM0_2_IRQHandler(void)
{
    if(TIM2_Isr)
    TIM2_Isr();
    NVIC_ClearPendingIRQ(TIM0_2_IRQn);
}

void TIM0_3_IRQHandler(void)
{
    if(TIM3_Isr)
    TIM3_Isr();
    NVIC_ClearPendingIRQ(TIM0_3_IRQn);
}

void TIM0_4_IRQHandler(void)
{
    if(TIM4_Isr)
    TIM4_Isr();
    NVIC_ClearPendingIRQ(TIM0_4_IRQn);
}

void TIM0_5_IRQHandler(void)
{

    if(TIM5_Isr)
    TIM5_Isr();

     TIM_ClearITPendingBit(TIMM0, TIM_5);
    NVIC_ClearPendingIRQ(TIM0_5_IRQn);
}


void TIM0_6_IRQHandler(void)
{
    if(TIM6_Isr)
    TIM6_Isr();
    NVIC_ClearPendingIRQ(TIM0_6_IRQn);

}

void TIM0_7_IRQHandler(void)
{
    if(TIM7_Isr)
    TIM7_Isr();
    NVIC_ClearPendingIRQ(TIM0_7_IRQn);
}

// 初始化GPIO中断模块
void gpio_int_init(void)
{
    static int gpio_int_inited = 0;
    if (gpio_int_inited) return;

    memset(GPIO_A_Isr, 0, sizeof(GPIO_A_Isr));
    memset(GPIO_B_Isr, 0, sizeof(GPIO_B_Isr));
    memset(GPIO_C_Isr, 0, sizeof(GPIO_C_Isr));
    memset(GPIO_D_Isr, 0, sizeof(GPIO_D_Isr));
    memset(GPIO_E_Isr, 0, sizeof(GPIO_E_Isr));

    NVIC_DisableIRQ(EXTI0_IRQn);
    NVIC_DisableIRQ(EXTI1_IRQn);
    NVIC_DisableIRQ(EXTI2_IRQn);
    NVIC_DisableIRQ(EXTI3_IRQn);
    NVIC_DisableIRQ(EXTI4_IRQn);
    NVIC_DisableIRQ(TIM0_0_IRQn);
    NVIC_DisableIRQ(TIM0_1_IRQn);
    NVIC_DisableIRQ(TIM0_2_IRQn);
    NVIC_DisableIRQ(TIM0_3_IRQn);
    NVIC_DisableIRQ(TIM0_4_IRQn);
    NVIC_DisableIRQ(TIM0_5_IRQn);
    NVIC_DisableIRQ(TIM0_6_IRQn);
    //NVIC_DisableIRQ(TIM0_7_IRQn);

    NVIC_SetPriority(EXTI0_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(EXTI1_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(EXTI2_IRQn, ICC_IO_PRIORITY);
    NVIC_SetPriority(EXTI3_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(EXTI4_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_0_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_1_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_2_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_3_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_4_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_5_IRQn, NORMAL_IO_PRIORITY);
    NVIC_SetPriority(TIM0_6_IRQn, NORMAL_IO_PRIORITY);
    //NVIC_SetPriority(TIM0_7_IRQn, NORMAL_IO_PRIORITY);


    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(TIM0_0_IRQn);
    NVIC_EnableIRQ(TIM0_1_IRQn);
    NVIC_EnableIRQ(TIM0_2_IRQn);
    NVIC_EnableIRQ(TIM0_3_IRQn);
    NVIC_EnableIRQ(TIM0_4_IRQn);
    NVIC_EnableIRQ(TIM0_5_IRQn);
    NVIC_EnableIRQ(TIM0_6_IRQn);

    //EXTI_LineConfig(EXTI_Line2, EXTI_PinSource1, EXTI_Trigger_Falling);
    //NVIC_EnableIRQ(TIM0_7_IRQn);

    gpio_int_inited = 1;
}

void gpio_isr_install(gpio_group_t gpio_group, unsigned int id, callback_isr_cb callback)
{
    if (callback == NULL) {
        return;
    }

    if (gpio_group > GPIO_GROUP_E || id >= MAX_GPIO_PER_GROUP) {
        return;
    }

    switch (gpio_group)
    {
        case GPIO_GROUP_A:
        GPIO_A_Isr[id] = callback;
        NVIC_EnableIRQ(EXTI0_IRQn);
        break;

        case GPIO_GROUP_B:
        GPIO_B_Isr[id] = callback;
        NVIC_EnableIRQ(EXTI1_IRQn);
        break;

        case GPIO_GROUP_C:
        GPIO_C_Isr[id] = callback;
        NVIC_EnableIRQ(EXTI2_IRQn);
        break;

        case GPIO_GROUP_D:
        GPIO_D_Isr[id] = callback;
        NVIC_EnableIRQ(EXTI3_IRQn);
        break;

        case GPIO_GROUP_E:
        GPIO_E_Isr[id] = callback;
        NVIC_EnableIRQ(EXTI4_IRQn);
    }
}

void tim_isr_install(tim_group_t tim_group, callback_isr_cb callback)
{
    if (callback == NULL) {
        return;
    }

    if (tim_group > TIM0_GROUP7) {
        return;
    }

    switch (tim_group)
    {
        case TIM0_GROUP0:
        TIM0_Isr = callback;
        NVIC_EnableIRQ(TIM0_0_IRQn);

        case TIM0_GROUP1:
        TIM1_Isr = callback;
        NVIC_EnableIRQ(TIM0_1_IRQn);

        case TIM0_GROUP2:
        TIM2_Isr = callback;
        NVIC_EnableIRQ(TIM0_2_IRQn);

        case TIM0_GROUP3:
        TIM3_Isr = callback;
        NVIC_EnableIRQ(TIM0_3_IRQn);

        case TIM0_GROUP4:
        TIM4_Isr = callback;
        NVIC_EnableIRQ(TIM0_4_IRQn);

        case TIM0_GROUP5:
        TIM5_Isr = callback;
        NVIC_EnableIRQ(TIM0_5_IRQn);

        case TIM0_GROUP6:
        TIM6_Isr = callback;
        NVIC_EnableIRQ(TIM0_6_IRQn);

        case TIM0_GROUP7:
        TIM7_Isr = callback;
        NVIC_EnableIRQ(TIM0_7_IRQn);
        break;
    }
}

// 使能指定GPIO的中断
void gpio_enable_interrupt(gpio_group_t gpio_group, unsigned int id, gpio_int_trigger_t trigger_type)
{
    if (gpio_group > GPIO_GROUP_E || id >= MAX_GPIO_PER_GROUP) {
        return;
    }

    switch (trigger_type) {
        case GPIO_INT_TRIGGER_NONE:
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE &= (~(0x03 << (id * 2)));
        break;
        case GPIO_INT_TRIGGER_RISING:
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE &= (~(0x03 << (id * 2)));
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE |= (0x01 << (id * 2));
        break;

        case GPIO_INT_TRIGGER_FALLING:
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE &= (~(0x03 << (id * 2)));
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE |= (0x02 << (id * 2));
        break;

        case GPIO_INT_TRIGGER_RISING_FALLING:
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE &= (~(0x03 << (id * 2)));
        GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE |= (0x03 << (id * 2));
        break;
    }
}

// 禁能指定GPIO的中断
void gpio_disable_interrupt(gpio_group_t gpio_group, unsigned int id)
{
    if (gpio_group > GPIO_GROUP_E || id >= MAX_GPIO_PER_GROUP) {
        return;
    }

    GPIO->INTP_TYPE_STA[gpio_group].INTP_TYPE &= (~(0x03 << (id * 2)));
}

// 清除指定GPIO的中断
void gpio_clear_interrupt(gpio_group_t gpio_group, unsigned int id)
{
    if (gpio_group > GPIO_GROUP_E || id >= MAX_GPIO_PER_GROUP) {
        return;
    }

    GPIO->INTP_TYPE_STA[gpio_group].INTP_STA = (1 << id);
}

