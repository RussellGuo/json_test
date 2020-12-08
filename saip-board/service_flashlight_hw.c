/*
 * service_flashlight_hw.c
 *
 *
 * This is a stub module, which is used to develop flashlight services on the start board. The code is
 * copied from the LED module and slightly modified for use in the R&D process only.
 *
 * hardware-depended part of flashlight service for the start-board,
 *  only used in the development process.
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#include "service_flashlight.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"

// const of this module
#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PIN   GPIO_PIN_8


// The hardware initialization function required for the operation of the flashlight module.
void flashlight_hw_init(void)
{
    /* configure LED GPIO port */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,LED_GPIO_PIN);
}


// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will loop indefinitely
// parameters:
// [in] flashlight_mode, flashlight_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the flashlight mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.
// Note: On the start board, the factory test of the LED is simulated with low flashing LED.
#define START_BOARD_LED_FLASH_HALF_PERIOD 256
void flashlight_running_until_new_request_arrived(uint32_t flashlight_mode, uint32_t flashlight_mode_param, uint32_t new_request_flag)
{
    (void)flashlight_mode_param;

    if (flashlight_mode == MCU_MODULE_OFF) {
        // OFF mode, turn off the LED
        gpio_bit_write(LED_GPIO_PORT, LED_GPIO_PIN, RESET);

        //  and wait the configuration changed flag
        int32_t wait = osThreadFlagsWait(new_request_flag, osFlagsWaitAny, osWaitForever);
        if (wait == new_request_flag)
            return;
    }

    if (flashlight_mode == FLASHLIGHT_MODE_FACTORY_TEST) {
        // factory test
        for(bool value = 0;; value ^= 1) {
            // toggle the LED
            gpio_bit_write(LED_GPIO_PORT, LED_GPIO_PIN, value ? SET : RESET);

            // wait the flag or the timer out
            uint32_t wait = osThreadFlagsWait(new_request_flag, osFlagsWaitAny, START_BOARD_LED_FLASH_HALF_PERIOD);
            if (wait == new_request_flag) {
                return;
            }
            // timer out, push to the next phase of the flashlight
        }
    }
}
