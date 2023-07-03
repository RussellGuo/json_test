#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/// 包含运行平台的头文件
#include "mhscpu.h"
#include "mhscpu_exti.h"
#include "mhscpu_gpio.h"
#include "mhscpu_uart.h"
#include "mhscpu_exti.h"
#include "mhscpu_timer.h"

void bt_pin_init(void);
void bt_power_on(void);
void bt_power_off(void);
void bt_uart_configuration(void);
void bt_timer_configuration(void);
void hal_uart_set_baud (uint32_t baud);
void mcu_wake_bt(uint8_t state);

#endif

