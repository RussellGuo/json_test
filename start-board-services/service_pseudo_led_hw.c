/*
 * service_pseudo_led_hw.c
 *
 * hardware-depended part of LED service for the start-board,
 *  only used in the development process.
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#if defined(GD32E103R_START)

#include "service_led.h"
#include "semantic_api.h"
#include "mcu-led-mode.h"
#include "mcu-common.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"


// const of this module
#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PIN   GPIO_PIN_8


// The hardware initialization function required for the operation of the LED module.
void led_hw_init(void)
{
    /* configure LED GPIO port */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,LED_GPIO_PIN);
}

// turn on/off of the led
// parameters:
//  [in]on, enable/disable
// return value:
//  true means success, otherwise failure
static bool turn_led(bool on)
{
    gpio_bit_write(LED_GPIO_PORT, LED_GPIO_PIN, on ? SET : RESET);
    return true;
}

// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will loop indefinitely
// parameters:
// [in] led_mode, led_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the LED mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.
// Note: On the start board, the factory test of the LED is simulated with a fast flashing LED.
#define START_BOARD_LED_FLASH_HALF_PERIOD 128
void led_running_until_new_request_arrived(uint32_t led_mode, uint32_t led_mode_param, uint32_t new_request_flag)
{
    (void)led_mode_param;

    if (led_mode == MCU_MODULE_OFF) {
        // OFF mode, turn off the LED
        turn_led(false);

        //  and wait the configuration changed flag
        bool flag_be_set = wait_flag(new_request_flag);
        if (flag_be_set)
            return;
    }

    if (led_mode == LED_MODE_FACTORY_TEST) {
        // factory test
        for(bool value = 0;; value ^= 1) {
            // toggle the LED
            turn_led(value);

            // wait the flag or the timer out
            bool flag_be_set = wait_flag_sleep(new_request_flag, START_BOARD_LED_FLASH_HALF_PERIOD);
            if (flag_be_set) {
                return;
            }
            // timer out, push to the next phase of the LED
        }
    }
}

#endif
