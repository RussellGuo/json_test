/*
 * mcu-hw-common.h
 *
 * common routines for MCU hardware
 *
 *  Created on: Jan 14, 2021
 *      Author: Guo Qiang
 */

#ifndef __MCU_HW_COMMON_H__
#define __MCU_HW_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "gd32e10x.h"
#include <stdlib.h>

// PIN description
struct mcu_pin_t {
    uint32_t pin_port;   // likes GPIOA
    uint32_t pin_no;     // likes GPIO_PIN_1
    uint32_t pin_mode;   // likes GPIO_MODE_OUT_PP
};

// setup pins with pin description array
// [in] pin_tab, ptr to a pin description array
// [in] tab_len, array size
// return value: none
void setup_pins(const struct mcu_pin_t *pin_tab, size_t tab_len);

// turn on RCUs with an array
// [in]rcu_tab, ptr to an array of RCUs, likes { RCU_GPIOB, RCU_I2C0 }
// [in] tab_len, array size
// return value: none
void enable_rcus(const rcu_periph_enum *rcu_tab, size_t tab_len);


#ifdef __cplusplus
}
#endif

#endif // __MCU_HW_COMMON_H__
