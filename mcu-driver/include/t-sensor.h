/*
 * t-sensor.h
 *
 * the TI temperature sensor driver (TI TMP75)
 * Note: this T-sensor can be accessed only in factory test mode
 *
 *  Created on: Jan 16, 2021
 *      Author: Guo Qiang
 */

#ifndef __T_SENSOR_H__
#define __T_SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// In order not to lose the accuracy of the sensor,
// the temperature's unit is not Celsius, but 1/256 of Celsius.
typedef int16_t temperature_t;

static inline int temperature_in_c_degree(temperature_t temperature)
{
    return temperature / 256; // unit translate
}


// get temperature from the T-sensor via I2C
// parameter:
//  [out] temperature: ptr of the temperature in 1/256 of Celsius
// return value: true means success, otherwise failure
bool get_temperature(temperature_t *temperature);

#ifdef __cplusplus
}
#endif

#endif // __T_SENSOR_H__
