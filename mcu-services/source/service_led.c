/*
 * service_led.c
 *
 * Implementation of MCU LED service module
 * MCU LED service module, this module will run autonomously, SaCP can configure
 *  the mode and mode parameters at any time. Refer to the documentation
 * "SaIP communication protocol between the host and the MCU" for the configuration method.
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#include "service_led.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"

// flag for thread
#define MODE_CHANGED_FLAG 1

// to keep the mode and mode parameter at running
static serial_datagram_item_t led_mode, led_mode_param;

// service main loop
__NO_RETURN static void led_thread(void *argument)
{
    (void)argument;
    led_hw_init();

    for(;;) {
        // call the hardware-depended running function
        led_running_until_new_request_arrived(led_mode, led_mode_param, MODE_CHANGED_FLAG);
    }
}

// stack for the thread
#if !defined(STACK_SIZE_OF_SERVICE_LED_THREAD)
#define STACK_SIZE_OF_SERVICE_LED_THREAD 1024
#endif

__ALIGNED(8) static uint8_t stack_of_thread[STACK_SIZE_OF_SERVICE_LED_THREAD];

// attributes for the thread
static const osThreadAttr_t ThreadAttr_LED = {
    .name = "LED_Thread",
    .stack_mem  = stack_of_thread,
    .stack_size = sizeof(stack_of_thread),
};


static osThreadId_t tid_led;

// start the LED service, should be invoked at the beginning.
// Return value:
//   true means success, otherwise failure
bool init_thread_of_service_led(void)
{
    // just create a main loop thread.
    tid_led = osThreadNew(led_thread, NULL, &ThreadAttr_LED);
    return tid_led !=NULL;
}


// ReplyToSetLedConfig, the function is in the API layer of the host-MCU communication
//  protocol and runs in the protocol receiving thread. It is used to notify the execution
//  service (after checking the validity of the parameters). See the documentation for details.
void ReplyToSetLedConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param,
       res_error_code_t *error_code, serial_datagram_item_t seq)
{
    rpc_log(LOG_INFO, "LED mode: %x mode parameter: %x", mode, mode_param);
    (void)seq;

    // checking the validity of the mode and mode parameter
    if (tid_led == NULL) {
        *error_code = ERR_NOT_READY;
        return;
    }

    *error_code = NO_ERROR;

    switch(mode) {

    case MCU_MODULE_OFF:
        if (mode_param != 0) {
            *error_code = ERR_PARAM;
            return;
        }
        break;

    case LED_MODE_FACTORY_TEST:
        break;

    case LED_MODE_USER1:
    case LED_MODE_USER2:
        *error_code = ERR_NO_IMPL;
        break;

    default:
        *error_code = ERR_MODE;
        return;
    }

    if (*error_code == NO_ERROR) {
        // checking passed, notify the execution service
        led_mode = mode;
        led_mode_param = mode_param;
        osThreadFlagsSet(tid_led, MODE_CHANGED_FLAG);
    }
}
