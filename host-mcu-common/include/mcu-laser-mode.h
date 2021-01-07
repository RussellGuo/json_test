/*
 * mcu-laser-mode.h
 *
 * definition of laser mode and mode parameter. those value will be used in the  protocol between the host and the MCU
 *
 *  Created on: Jan 7, 2021
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */


#ifndef __MCU_LASER_MODE_H_
#define __MCU_LASER_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

// MODE USER1/2 TBD
#define LASER_MODE_USER1                0x11
#define LASER_MODE_USER2                0x12

// for factory
#define LASER_MODE_FACTORY_TEST         0x18

// TODO: USER MODE parameter definition

#ifdef __cplusplus
}
#endif

#endif // __MCU_LASER_MODE_H_
