/*
 * service_db9_pin_hw.c
 *
 * hardware-depended part of set db9 pin service for the SAIP
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 *
 *  Created on: Jan 25, 2021
 *      Author: Guo Qiang
 */

#include "semantic_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "mcu-hw-common.h"

// DB9 pin table
static const struct mcu_pin_t db9_pin_tab[] = {
    { GPIOB, GPIO_PIN_4 }, // GPIO4
    { GPIOB, GPIO_PIN_5 }, // GPIO7
    { GPIOA, GPIO_PIN_14  }, // GPIO6
    { GPIOC, GPIO_PIN_13  }, // GPIO13
    { GPIOC, GPIO_PIN_14  }, // GPIO8
    { GPIOB, GPIO_PIN_7  }, // I2C0_SDA
    { GPIOB, GPIO_PIN_6  }, // I2C0_SCL
    { GPIOB, GPIO_PIN_12  },
    { GPIOB, GPIO_PIN_13  },
    { GPIOB, GPIO_PIN_3  },
    { GPIOA, GPIO_PIN_15  },
    { GPIOA, GPIO_PIN_13  },
    { GPIOA, GPIO_PIN_0  },
};

static const rcu_periph_enum rpu_mcu_tab[] = {
    RCU_GPIOA,
    RCU_GPIOB,
    RCU_GPIOC,
    RCU_AF,
};

#define DB9_PIN_COUNT (sizeof(db9_pin_tab) / sizeof(db9_pin_tab[0]))

void db9_hw_init(void)
{
    /* configure RCU of LED GPIO port ON*/
    enable_rcus(rpu_mcu_tab, sizeof(rpu_mcu_tab) / sizeof(rpu_mcu_tab[0]));
    gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);
}

// ReplyToConfigDb9Pin, ReplyToSetDb9Pin and ReplyToGetDb9Pin, those function are in the API layer of
// the host-MCU communication protocol and runs in the protocol receiving thread. It will be called by
// protocol receiving thread (after checking the validity of the parameters), then do all the test, and store the result,
// and then the receiving thread will send them to the host as a respose. See the documentation for details.
// This service does not need a separate thread, because the response operation is very fast
// and will not block the receiving thread of the protocol.

// to configue the DB9 Pin with input/output mode
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in] pin_no, pin index of the DB9 GPIO table
//   [in] pin_config, valid values: GPIO_MODE_IN_FLOATING, GPIO_MODE_IPD, GPIO_MODE_IPU, GPIO_MODE_OUT_OD, GPIO_MODE_OUT_PP
//   [in] seq, the request seq
void ReplyToConfigDb9Pin(
    serial_datagram_item_t pin_no,
    serial_datagram_item_t pin_config,
    res_error_code_t *error_code,  serial_datagram_item_t seq)
{
    (void)seq;
    // check parameter pin_no
    if (pin_no >= DB9_PIN_COUNT) {
        *error_code = ERR_PARAM;
        return;
    }
    db9_hw_init();

    // check parameter pin_config
    bool is_output;
    switch(pin_config) {
    case GPIO_MODE_IN_FLOATING:
    case GPIO_MODE_IPD:
    case GPIO_MODE_IPU:
        is_output = false;
        break;

    case GPIO_MODE_OUT_OD:
    case GPIO_MODE_OUT_PP:
        is_output = true;
        break;

    default:
        *error_code = ERR_PARAM;
        return;
    }

    // configure it
    *error_code = NO_ERROR;
    const struct mcu_pin_t *p = db9_pin_tab + pin_no;
    gpio_init(p->pin_port, pin_config, GPIO_OSPEED_50MHZ, p->pin_no);
    if (is_output) {
        write_pin(p, false); // first configure a pin with output, then should set it with low
    }
    rpc_log(LOG_VERBOSE, "config pin #%u with mode %02X", pin_no, pin_config);
}

// to set value of the DB9 pin
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in] pin_no, pin index of the DB9 GPIO table
//   [in] pin_value, false/true for setting value low/high of the pin
//   [in] seq, the request seq
void ReplyToSetDb9Pin(serial_datagram_item_t pin_no, bool pin_value, res_error_code_t *error_code,  serial_datagram_item_t seq)
{
    (void)seq;
    // check parameters
    if (pin_no >= DB9_PIN_COUNT) {
        *error_code = ERR_PARAM;
        return;
    }

    db9_hw_init();
    // set it
    const struct mcu_pin_t *p = db9_pin_tab + pin_no;
    *error_code = NO_ERROR;
    write_pin(p, pin_value);
    rpc_log(LOG_VERBOSE, "set pin #%u with %s", pin_no, pin_value ? "high" : "low");
}

// to get the value of the BD9 pin
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in] pin_no, pin index of the DB9 GPIO table
//   [out] pin_value, the ptr to false/true for setting value low/high of the pin
//   [in] seq, the request seq
void ReplyToGetDb9Pin(serial_datagram_item_t pin_no, bool *pin_value, res_error_code_t *error_code,  serial_datagram_item_t seq)
{
    (void)seq;
    // check parameters
    if (pin_no >= DB9_PIN_COUNT) {
        *error_code = ERR_PARAM;
        return;
    }

    db9_hw_init();
    // get it
    *error_code = NO_ERROR;
    const struct mcu_pin_t *p = db9_pin_tab + pin_no;
    *pin_value = read_pin(p);
    rpc_log(LOG_VERBOSE, "get pin #%u: %s", pin_no, *pin_value ? "high" : "low");
}
