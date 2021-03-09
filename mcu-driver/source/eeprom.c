/*
 * eeprom.c
 *
 * the EEPROM driver (GT24C64A)
 * Note: this EEPROM can be accessed only in factory test mode
 *
 *  Created on: Jan 18, 2021
 *      Author: Guo Qiang
 */

// the EEPROM was originally visited on SACP, but there is no SACP in the PCBA stage of the production line.
// For this reason, the production line will connect the I2C of the MCU and the I2C of the EEPROM (and T-Sensor)
// through a fixture, so the EEPROM can also be accessed.

#include "cmsis_os2.h"
#include "gd32e10x.h"
#include "i2c-base.h"

// values from GT24C64A and circuit
#define EEPROM_ADDRESS 0xa2
#define I2C_FOR_EEPROM I2C0

// read data from the EEPROM
// parameters:
//   [out] byte: pointer to the buffer that receives the data read from the EEPROM
//   [in]  read_address: EEPROM's internal address to start reading from
// reture valalue:
//   true means success, otherwise failure
bool eeprom_byte_read(uint8_t* byte, uint8_t read_address)
{
    bool ret;
    ret = i2c_hw_init(I2C_FOR_EEPROM); // init the bus
    if (!ret) {
        return ret; // the I2C is in use
    }

    if (ret) ret = i2c_flag_get(I2C_FOR_EEPROM, I2C_FLAG_I2CBSY) == RESET;           // test if the bus is idle

    if (ret) i2c_start_on_bus(I2C_FOR_EEPROM);                                       // start bit send
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_SBSEND);               // sent

    if (ret) i2c_master_addressing(I2C_FOR_EEPROM, EEPROM_ADDRESS, I2C_TRANSMITTER); // send I2C addr on the bus
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);              // wait it be sent
    if (ret) i2c_flag_clear(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);                       // clean flag for transmitting

    // if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_TBE);               // it's not necessary since bus is idle

    if (ret) i2c_data_transmit(I2C_FOR_EEPROM, 0x00);                                // send high byte of address
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_TBE);                  // wait it be sent
    if (ret) i2c_data_transmit(I2C_FOR_EEPROM, read_address);                        // send low byte of address
    // if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_TBE);               // wait it be sent

    if (ret) i2c_start_on_bus(I2C_FOR_EEPROM);                                       // start bit again before receiving
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_SBSEND);               // sent

    if (ret) i2c_master_addressing(I2C_FOR_EEPROM, EEPROM_ADDRESS, I2C_RECEIVER);    // begin to receive
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);              // sent
    if (ret) i2c_flag_clear(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);                       // clean flag for receiving

    if (ret) i2c_ack_config(I2C_FOR_EEPROM, I2C_ACK_DISABLE);                        // only 1 byte to read, then no ack
    if (ret) i2c_stop_on_bus(I2C_FOR_EEPROM);                                        // and stop should be sent

    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_RBNE);                 // wait for byte ready
    if (ret) *byte = i2c_data_receive(I2C_FOR_EEPROM);                               // got it

    i2c_hw_deinit(I2C_FOR_EEPROM);                                                   // release I2C whether OK/failed

    return ret;
}

// write one byte to the I2C EEPROM
// parameters:
//   [in] byte: data to be written to the EEPROM
//   [in] write_address: EEPROM's internal address to write to
// reture valalue:
//   true means success, otherwise failure
bool eeprom_byte_write(uint8_t byte, uint8_t write_address)
{
    bool ret;
    ret = i2c_hw_init(I2C_FOR_EEPROM);                                       // init the bus
    if (!ret) {
        return ret; // the I2C is in use
    }

    if (ret) ret = i2c_flag_get(I2C_FOR_EEPROM, I2C_FLAG_I2CBSY) == RESET;           // test if the bus is idle

    if (ret) i2c_start_on_bus(I2C_FOR_EEPROM);                                       // start bit send
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_SBSEND);               // sent

    if (ret) i2c_master_addressing(I2C_FOR_EEPROM, EEPROM_ADDRESS, I2C_TRANSMITTER); // send I2C addr on the bus
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);              // wait it be sent
    if (ret) i2c_flag_clear(I2C_FOR_EEPROM, I2C_FLAG_ADDSEND);                       // clean flag for transmitting

    // if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_TBE);               // it's not necessary since bus is idle
    if (ret) i2c_data_transmit(I2C_FOR_EEPROM, 0x00);                                // send high byte of address
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_BTC);                  // sent
    if (ret) i2c_data_transmit(I2C_FOR_EEPROM, write_address);                       // send low byte of address
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_BTC);                  // sent
    if (ret) i2c_data_transmit(I2C_FOR_EEPROM, byte);                                // send value
    if (ret) ret = wait_for_i2c_flag(I2C_FOR_EEPROM, I2C_FLAG_BTC);                  // sent

    i2c_stop_on_bus(I2C_FOR_EEPROM);                                                 // send stop condition to release bus
    osDelay(1);                                                                      // sent
    i2c_hw_deinit(I2C_FOR_EEPROM);                                                   // release I2C whether OK/failed

    return ret;
}
