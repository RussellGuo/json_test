#include "hal_gpio.h"
#include "mhscpu_gpio.h"

// 将group号转换为寄存器
static GPIO_TypeDef *gpio_group_reg[5] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE
};

// 配置GPIO的复用功能
void gpio_config_alt(gpio_group_t group, unsigned int pin, unsigned int alt)
{
    GPIO_RemapTypeDef alt_map[4] = {
        GPIO_Remap_0,
        GPIO_Remap_1,
        GPIO_Remap_2,
        GPIO_Remap_3
    };

    if ((group > GPIO_GROUP_E) || (alt >= sizeof(alt_map)/sizeof(alt_map[0]))) {
        return;
    }

    GPIO_PinRemapConfig(gpio_group_reg[group], BIT(pin), alt_map[alt]);
}

// 配置GPIO的方向为输入
void gpio_config_dir_input(gpio_group_t group, unsigned int pin, gpio_pull_t pullstate)
{
    if (group > GPIO_GROUP_E) {
        return;
    }

    gpio_group_reg[group]->OEN |= BIT(pin);

    switch (pullstate) {
        case HAL_GPIO_PULL_UP:
        gpio_group_reg[group]->PUE |= BIT(pin);
        break;

        case HAL_GPIO_FLOATING:
        gpio_group_reg[group]->PUE &= (~BIT(pin));
        break;

        default:
        break;
    }
}

// 配置GPIO的方向为输出
void gpio_config_dir_output(gpio_group_t group, unsigned int pin, unsigned int level)
{
    if (group > GPIO_GROUP_E) {
        return;
    }

    gpio_group_reg[group]->OEN &= (~BIT(pin));
    gpio_group_reg[group]->PUE &= (~BIT(pin));

    switch (level) {
        case 0:
        gpio_group_reg[group]->IODR &= (~BIT(pin));
        break;

        default:
        gpio_group_reg[group]->IODR |= BIT(pin);
        break;
    }
}

// 获取GPIO的输入电平
int gpio_get_val(gpio_group_t group, unsigned int pin)
{
    if (group > GPIO_GROUP_E) {
        return 0;
    }

    return ((((gpio_group_reg[group]->IODR >> 16) & 0xFFFF) & BIT(pin)) == 0)?0:1;
}

// 设置GPIO的输出电平
void gpio_set_val(gpio_group_t group, unsigned int pin, unsigned int level)
{
    if (group > GPIO_GROUP_E) {
        return;
    }

    if (level) {
        gpio_group_reg[group]->BSRR = BIT(pin);
    } else {
        gpio_group_reg[group]->BSRR = (BIT(pin) << 16);
    }
}

