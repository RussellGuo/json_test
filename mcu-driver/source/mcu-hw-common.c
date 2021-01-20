/*
 * mcu-hw-common.c
 *
 * common routines for MCU hardware
 *
 *  Created on: Jan 14, 2021
 *      Author: Guo Qiang
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
        gpio_init(pin->pin_port, pin->pin_mode, GPIO_OSPEED_50MHZ, pin->pin_no);
    }
}


// turn on RCUs with an array
// [in]rcu_tab, ptr to an array of RCUs, likes { RCU_GPIOB, RCU_I2C0 }
// [in] tab_len, array size
// return value: none
void enable_rcus(const rcu_periph_enum *rpu_tab, size_t tab_len)
{
    for (size_t i = 0; i < tab_len; i++) {
        const rcu_periph_enum rpu = rpu_tab[i];
        rcu_periph_clock_enable(rpu);
    }
}
