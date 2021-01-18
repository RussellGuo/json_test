/*
 * t-sensor.c
 *
 * the TI temperature sensor driver (TI TMP75)
 * Note: this T-sensor can be accessed only in factory test mode
 *
 *  Created on: Jan 16, 2021
 *      Author: Guo Qiang
 */

#include "t-sensor.h"
#include "i2c-base.h"

#include "gd32e10x.h"
#include "cmsis_os2.h"

// TI TMP75
#define TSENSOR_ADDRESS 0x9e
#define I2C_FOR_TSENSOR I2C0

// get temperature from the T-sensor via I2C
// parameter:
//  [out] temperature: ptr to the temperature in 1/256 of Celsius
// return value: true means success, otherwise failure
bool get_temperature(temperature_t *temperature)
{
    bool ret = true;
    uint8_t temp_buff[2];  // to store temp bytes


    if(ret) ret = i2c_hw_init(I2C_FOR_TSENSOR);                                       // init the bus

    if(ret) ret = i2c_flag_get(I2C_FOR_TSENSOR, I2C_FLAG_I2CBSY) == RESET;            // test if the bus is idle
    if(ret) i2c_start_on_bus(I2C_FOR_TSENSOR);                                        // start bit send
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_SBSEND);                // sent

    if(ret) i2c_master_addressing(I2C_FOR_TSENSOR, TSENSOR_ADDRESS, I2C_TRANSMITTER); // send I2C addr on the bus
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_ADDSEND);               // wait it be sent
    if(ret) i2c_flag_clear(I2C_FOR_TSENSOR, I2C_FLAG_ADDSEND);                        // clean flag for transmitting

    // if(ret) ret = i2c_flag_get(I2C_FOR_TSENSOR, I2C_FLAG_TBE) == SET;              // it's not necessary since bus is idle

    if(ret) i2c_data_transmit(I2C_FOR_TSENSOR, 0x00);                                 // sensor register 0
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_TBE);                   // sending complete

    if(ret) i2c_start_on_bus(I2C_FOR_TSENSOR);                                        // start bit again before receiving
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_SBSEND);                // sent

    if(ret) i2c_master_addressing(I2C_FOR_TSENSOR, TSENSOR_ADDRESS, I2C_RECEIVER);    // begin to receive
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_ADDSEND);               // sent
    if(ret) i2c_flag_clear(I2C_FOR_TSENSOR, I2C_FLAG_ADDSEND);                        // clean flag for receiving

    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_RBNE);                  // wait for byte 0 ready
    if(ret) temp_buff[0] = i2c_data_receive(I2C_FOR_TSENSOR);                         // got it
    if(ret) ret = wait_for_i2c_flag(I2C_FOR_TSENSOR, I2C_FLAG_RBNE);                  // wait for byte 1 ready
    if(ret) temp_buff[1] = i2c_data_receive(I2C_FOR_TSENSOR);                         // got it


    i2c_hw_deinit(I2C_FOR_TSENSOR);                                                   // release I2C whether OK/failed
    if(ret) {
        *temperature = (int16_t)(uint16_t)( (temp_buff[0] << 8) | temp_buff[1]);      // combine data according the spec
    }
    return ret;
}
