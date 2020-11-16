#ifndef __USER_KEY_ISR_H__
#define __USER_KEY_ISR_H__

#include <stdint.h>
#include <stdbool.h>


// API
typedef void (*user_key_callback_t)(bool pressed);

// cb != NULL, a callback of the key interrupt.
// cb == NULL, uninstall the last init
// return non-zero if succeeded
bool init_user_key_isr(user_key_callback_t cb);


// const of this module

/* user key */
#define USER_KEY_PIN                     GPIO_PIN_0
#define USER_KEY_GPIO_PORT               GPIOA
#define USER_KEY_GPIO_CLK                RCU_GPIOA
#define USER_KEY_EXTI_LINE               EXTI_0
#define USER_KEY_EXTI_PORT_SOURCE        GPIO_PORT_SOURCE_GPIOA
#define USER_KEY_EXTI_PIN_SOURCE         GPIO_PIN_SOURCE_0
#define USER_KEY_EXTI_IRQn               EXTI0_IRQn

#endif  // __USER_KEY_ISR_H__
