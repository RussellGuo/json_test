/*
 * mcu-hw-common.h
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

#ifndef __MCU_HW_COMMON_H__
#define __MCU_HW_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "mhscpu.h"

// PIN description
//MH2101�� GPIO�� ʹ����GPIO_TypeDef *������ MODEʹ����GPIO_RemapTypeDef����
struct mcu_pin_t {
    GPIO_TypeDef *pin_port;   // likes GPIOA
    uint16_t pin_no;     // likes GPIO_PIN_1
    GPIO_RemapTypeDef pin_mode;   // likes GPIO_Remap_0
};

// setup pins with pin description array
// [in] pin_tab, ptr to a pin description array
// [in] tab_len, array size
// return value: none
void setup_pins(const struct mcu_pin_t *pin_tab, size_t tab_len);

//MH2101����GPIO_SetBits & GPIO_ResetBits����GPIO�������� ����ԭ����mhscpu_gpio.c
static inline void write_pin(const struct mcu_pin_t *pin, bool value)
{
      if(value)
      {
          GPIO_SetBits(pin->pin_port, pin->pin_no);
      }
      else
      {
          GPIO_ResetBits(pin->pin_port, pin->pin_no);
      }
}

//MH2101 GPIO״̬��ȡ����GPIO_ReadInputDataBit ԭ����mhscpu_gpio.c
static inline bool read_pin(const struct mcu_pin_t *pin)
{
    FlagStatus ret = (FlagStatus)GPIO_ReadInputDataBit(pin->pin_port, pin->pin_no);
    return ret == SET;
}

#ifdef __cplusplus
}
#endif

#endif // __MCU_HW_COMMON_H__
