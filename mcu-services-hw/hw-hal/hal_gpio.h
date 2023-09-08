#ifndef _HAL_GPIO_H
#define _HAL_GPIO_H

// GPIO硬件抽象层

// GPIO分组
typedef enum {
    GPIO_GROUP_A = 0, // PortA
    GPIO_GROUP_B = 1, // PortB
    GPIO_GROUP_C = 2, // PortC
    GPIO_GROUP_D = 3,  // PortD
    GPIO_GROUP_E = 4,  // PortD
} gpio_group_t;

//TIM分组
typedef enum {
    TIM0_GROUP0 = 0,   //TIM0_0
    TIM0_GROUP1 = 1,   //TIM0_1
    TIM0_GROUP2 = 2,   //TIM0_2
    TIM0_GROUP3 = 3,   //TIM0_3
    TIM0_GROUP4 = 4,   //TIM0_4
    TIM0_GROUP5 = 5,  //TIM0_5
    TIM0_GROUP6 = 6,  //TIM0_6
	  TIM0_GROUP7 = 7   //TIM0_7
} tim_group_t;

// GPIO上下拉状态
typedef enum {
    HAL_GPIO_FLOATING = 0,
    HAL_GPIO_PULL_UP = 1,
    HAL_GPIO_PULL_DOWN = 2
} gpio_pull_t;

// 配置GPIO的复用功能
// pin 范围 0-N
void gpio_config_alt(gpio_group_t group, unsigned int pin, unsigned int alt);

// 配置GPIO的方向为输入
// pin 范围 0-N
void gpio_config_dir_input(gpio_group_t group, unsigned int pin, gpio_pull_t pullstate);

// 配置GPIO的方向为输出
// pin 范围 0-N
void gpio_config_dir_output(gpio_group_t group, unsigned int pin, unsigned int level);

// 获取GPIO的输入电平
// pin 范围 0-N
int gpio_get_val(gpio_group_t group, unsigned int pin);

// 设置GPIO的输出电平
// pin 范围 0-N
void gpio_set_val(gpio_group_t group, unsigned int pin, unsigned int level);

#endif

