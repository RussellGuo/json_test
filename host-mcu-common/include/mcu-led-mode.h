/*
 * mcu-led-mode.h
 *
 * definition of LED mode and mode parameter. those value will be used in the  protocol between the host and the MCU
 *
 *  Created on: Dec 11, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */


#ifndef __MCU_LED_MODE_H_
#define __MCU_LED_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

// MODE USER1/2 TBD
#define LED_MODE_USER1                  0x01
#define LED_MODE_USER2                  0x02

// for factory
#define LED_MODE_FACTORY_TEST           0x08

// factory mode no need blink. just ON/OFF for each color and light

// compose each color and light into a parameter. for host
#define LED_MODE_PARAM_FOR_FACTORY_TEST(r0, g0, b0, r1, g1, b1) ( (r0) * 1 + (g0) * 2 + (b0) * 4 + (r1) * 8 + (g1) * 16 + (b1) * 32)

// extract ON/OFF for each color and light from mode parameter. for MCU side
#define LED_R0_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 0) & 1)
#define LED_G0_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 1) & 1)
#define LED_B0_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 2) & 1)
#define LED_R1_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 3) & 1)
#define LED_G1_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 4) & 1)
#define LED_B1_FROM_MODE_PARAM_FOR_FACTORY_TEST(param) (((param) >> 5) & 1)

// TODO: USER MODE parameter definition

#ifdef __cplusplus
}
#endif

#endif //__MCU_LED_MODE_H_
