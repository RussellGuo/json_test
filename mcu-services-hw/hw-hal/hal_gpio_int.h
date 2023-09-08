#ifndef _HAL_GPIO_INT_H
#define _HAL_GPIO_INT_H
#include "hal_gpio.h"
// GPIO 中断 HAL层

// GPIO中断触发方式
typedef enum {
    GPIO_INT_TRIGGER_NONE = 0x00,
    GPIO_INT_TRIGGER_RISING = 0x01,
    GPIO_INT_TRIGGER_FALLING = 0x02,
    GPIO_INT_TRIGGER_RISING_FALLING = 0x03
} gpio_int_trigger_t;

// GPIO中断处理函数原型
typedef void(*callback_isr_cb)(void);

// 初始化GPIO中断管理模块
// 仅需调用一次
void gpio_int_init(void);

//为指定的定时器安装中断函数
void tim_isr_install(tim_group_t tim_group, callback_isr_cb callback);
// 为指定GPIO安装中断处理函数
void gpio_isr_install(gpio_group_t gpio_group, unsigned int id, callback_isr_cb callback);

// 使能指定GPIO的中断
void gpio_enable_interrupt(gpio_group_t gpio_group, unsigned int id, gpio_int_trigger_t trigger_type);

// 禁用指定GPIO的中断
void gpio_disable_interrupt(gpio_group_t gpio_group, unsigned int id);

// 清除指定GPIO的中断
void gpio_clear_interrupt(gpio_group_t gpio_group, unsigned int id);

#endif

