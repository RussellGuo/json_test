/*
 * i2c-base.h
 *
 * common routines for I2C devices, such as flashlight, T-Sensor, EEPROM
 * Note: T-Sensor and EEPROM can be accessed only in factory test mode
 *
 *  Created on: Jan 15, 2021
 *      Author: Guo Qiang
 */

#ifndef __I2C_BASE_H__
#define __I2C_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "gd32e10x.h"

// make initial preparations for I2C bus access
// it should be invoked at an earlier time, for example in main()
// parameter: none
// return value: true means success, otherwise failure
bool init_i2c_buses(void);

// occupy an I2C bus and init it. i2c_hw_init() needs to be invoked
// at the beginning of the session of MCU and device, and i2c_hw_deinit()
// needs to be invoked at the end.
// parameter:
//   [in] i2c_no, should be I2C0 or I2C1
// return value: true means success, otherwise failure (mostly, another thread is also using this I2C)
bool i2c_hw_init(uint32_t i2c_no);

// deinit an I2C bus and release it. i2c_hw_init() needs to be invoked
// at the beginning of the session of MCU and device, and i2c_hw_deinit()
// needs to be invoked at the end.
// parameter:
//   [in] i2c_no, should be I2C0 or I2C1
// return value: true means success, otherwise failure
bool i2c_hw_deinit(uint32_t i2c_no);

// wait an I2C flag be set in 1ms. it's a busy loop waitting
// it will be invoked repeatedly during the MCU and device session
// parameters:
//   [in] i2c_no, should be I2C0 or I2C1
//   [in] flag, such as I2C_FLAG_SBSEND, I2C_FLAG_ADDSEND, I2C_FLAG_TBE, I2C_FLAG_RBNE
// return value: true means flag is set, otherwise timeout
bool wait_for_i2c_flag(uint32_t i2c, i2c_flag_enum flag);

#ifdef __cplusplus
}
#endif

#endif // __I2C_BASE_H__
