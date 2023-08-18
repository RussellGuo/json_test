/*
 * mcu-hw-common.c
 *
 * common routines for MCU hardware
 *
 *  Created on: Jan 14, 2021
 *      Author: Guo Qiang
 *
 *  Adapter MH2101 MCU hardware
 *
 *  Update on : Aug 11, 2023
 *      Author: Cao Meng
 */

#include "mcu-hw-common.h"

// setup pins with pin description array
// [in] pin_tab, ptr to a pin description array
// [in] tab_len, array size
// return value: none
void setup_pins(const struct mcu_pin_t *pin_tab, size_t tab_len)
{
    for (size_t i = 0; i < tab_len; i++) {
        const struct mcu_pin_t *pin = pin_tab + i;
        GPIO_PinRemapConfig(pin->pin_port, pin->pin_no,  pin->pin_mode); //MH2101 GPIO设置函数GPIO_PinRemapConfig 原型在mhscpu_gpio.c
    }
}
