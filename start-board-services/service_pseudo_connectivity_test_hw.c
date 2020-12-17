/*
 * service_pseudo_connectivity_test_hw.c
 *
 * hardware-depended part of connectivity-test service for the start-board,
 *  only used in the development process.
 *
 *  Created on: Dec 14, 2020
 *      Author: Guo Qiang
 */

#if defined(GD32E103R_START)

#include "semantic_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

// On the start board, we use the user key to simulate whether the
// production line test is passed: press it to pass, and it fails when it is not.

#define USER_KEY_PIN                     GPIO_PIN_0
#define USER_KEY_GPIO_PORT               GPIOA
#define USER_KEY_GPIO_CLK                RCU_GPIOA

// user key value fetch
static bool start_board_connectivity_test(void)
{
    /* enable the key clock */
    rcu_periph_clock_enable(USER_KEY_GPIO_CLK);
    rcu_periph_clock_enable(RCU_AF);

    /* configure key pin as input */
    gpio_init(USER_KEY_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USER_KEY_PIN);
    bool ret = gpio_input_bit_get(USER_KEY_GPIO_PORT, USER_KEY_PIN) == SET;
    return ret;
}

void ReplyToConnectivityTest(res_error_code_t *error_code, uint32_t *test_item_list, serial_datagram_item_t seq)
{
    (void)seq;
   *error_code = NO_ERROR;
    bool value = start_board_connectivity_test();

    for (int i = 0; i < CONNECTIVITY_TEST_RESULT_COUNT; i++) {
        test_item_list[i] = value;
    }
}

#endif
