#ifndef __TASK_LED_BLINK_H__
#define __TASK_LED_BLINK_H__

#include <stdint.h>
#include <stdbool.h>
#include "gd32e10x.h"


// API
bool init_thread_of_led_blink(void);


// const of this module
#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PIN   GPIO_PIN_8

#endif
