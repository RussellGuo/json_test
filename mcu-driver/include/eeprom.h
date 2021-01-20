/*
 * eeprom.h
 *
 * the EEPROM driver (GT24C64A)
 * Note: this EEPROM can be accessed only in factory test mode
 *
 *  Created on: Jan 16, 2021
 *      Author: Guo Qiang
 */

// the EEPROM was originally visited on SACP, but there is no SACP in the PCBA stage of the production line.
// For this reason, the production line will connect the I2C of the MCU and the I2C of the EEPROM (and T-Sensor)
// through a fixture, so the EEPROM can also be accessed.

#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// read data from the EEPROM
// parameters:
//   [out] byte: pointer to the buffer that receives the data read from the EEPROM
//   [in]  read_address: EEPROM's internal address to start reading from
// reture valalue:
//   true means success, otherwise failure
bool eeprom_byte_read(uint8_t* byte, uint8_t read_address);

// write one byte to the I2C EEPROM
// parameters:
//   [in] byte: data to be written to the EEPROM
//   [in] write_address: EEPROM's internal address to write to
// reture valalue:
//   true means success, otherwise failure
bool eeprom_byte_write(uint8_t byte, uint8_t write_address);


#ifdef __cplusplus
}
#endif

#endif // __EEPROM_H__
