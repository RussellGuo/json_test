/*
 * service_led_hw.c
 *
 * hardware-depended part of LED service for the saip board
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"
#include "service_led.h"
#include "mcu-led-mode.h"
#include "mcu-common.h"

// LED GPIO power control
static rcu_periph_enum rpu_tab[] = {
    RCU_GPIOA,
    RCU_GPIOB,
};

#define LED_COUNT 6

// LED GPIO PIN definition
static const struct {
    uint32_t pin_port;
    uint32_t pin_no;
} led_pin_tab[LED_COUNT] = {
    { GPIOB, GPIO_PIN_0 },  // LED0, red
    { GPIOA, GPIO_PIN_7 },  // LED0, green
    { GPIOA, GPIO_PIN_6 },  // LED0, blue
    { GPIOA, GPIO_PIN_5 },  // LED1, red
    { GPIOA, GPIO_PIN_4 },  // LED1, green
    { GPIOA, GPIO_PIN_3 },  // LED1, blue
};


// The hardware initialization function required for the operation of the LED module.
void led_hw_init(void)
{
    /* configure RCU of LED GPIO port ON*/
    for (int i = 0; i < sizeof(rpu_tab) / sizeof(rpu_tab[0]); i++) {
        rcu_periph_clock_enable(rpu_tab[i]);
    }

    /* configure LEDs GPIO port as output*/
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_init(led_pin_tab[i].pin_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, led_pin_tab[i].pin_no);
    }
}

// turn on/off of the LED
// parameters:
//  [in]light_no, index of the led_pin_tab
//  [in]light_on, enable/disable the light
// return value:
//  true means success, otherwise failure
static bool led_set_with_no(int light_no, bool light_on)
{
    rpc_log(LOG_VERBOSE, "led_set_with_no(%d, %d)", light_no, light_on);
    if (light_no >= LED_COUNT) {
        return false;
    }
    // locate the pin, set its value
    gpio_bit_write(led_pin_tab[light_no].pin_port, led_pin_tab[light_no].pin_no, light_on ? SET : RESET);
    return true;
}


// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will always wait
// parameters:
// [in] led_mode, led_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the LED mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.

// TODO: USER MODE
void led_running_until_new_request_arrived(uint32_t led_mode, uint32_t led_mode_param, uint32_t new_request_flag)
{
    (void)led_mode_param;

    if (led_mode == MCU_MODULE_OFF) {
        // OFF mode, turn off the LED
        for(int i = 0; i < LED_COUNT; i++) {
            led_set_with_no(i, false);
        }
    }

    if (led_mode == LED_MODE_FACTORY_TEST) {
        // factory test, extract ON/OFF for each color and light from mode parameter
        const bool led_request_onoff[LED_COUNT] = {
            LED_R0_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
            LED_G0_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
            LED_B0_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
            LED_R1_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
            LED_G1_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
            LED_B1_FROM_MODE_PARAM_FOR_FACTORY_TEST(led_mode_param),
        };

        // set every light
        for (int i = 0; i < LED_COUNT; i++) {
            led_set_with_no(i, led_request_onoff[i]);
        }
    }

    if (led_mode == LED_MODE_USER1) {
        // TODO: fill this
    }
    if (led_mode == LED_MODE_USER2) {
        // TODO: fill this
    }

    // waitting next request
    wait_flag(new_request_flag);
}
