/*
 * mcu-flashlight-mode.h
 *
 * definition of flashlight mode and mode parameter. those value will be used in the  protocol between the host and the MCU
 *
 *  Created on: Jan 7, 2021
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */


#ifndef __MCU_FLASHLIGHT_MODE_H_
#define __MCU_FLASHLIGHT_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

// MODE USER1/2 TBD
#define FLASHLIGHT_MODE_USER1           0x21
#define FLASHLIGHT_MODE_USER2           0x22

// for factory
#define FLASHLIGHT_MODE_FACTORY_TEST    0x28


// TODO: USER MODE parameter definition

#ifdef __cplusplus
}
#endif

#endif // __MCU_FLASHLIGHT_MODE_H_
