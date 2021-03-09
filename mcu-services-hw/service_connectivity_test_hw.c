/*
 * service_connectivity_test_hw.c
 *
 * hardware-depended part of connectivity-test service for the SAIP
 *
 *  Created on: Dec 14, 2020
 *      Author: Guo Qiang
 */

#include "semantic_api.h"
#include "mcu-connectivity-test-result.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "mcu-hw-common.h"

// for T-sensor test
#include "t-sensor.h"

#include "db9_init_for_factory.h"

// connectivity power control
static const rcu_periph_enum rpu_tab[] = {
    RCU_GPIOA,
    RCU_GPIOB,
    RCU_AF,
};

// The connectivity test is to test whether the DB9 pins are connected properly on the production line,
// and T-sensor is also done in this test.
// On the production line, a special test board is used to test the connectivity of DB9. Two DB9s are
// tested with 5 groups of independent circuits. Each group of circuits corresponds to power, ground,
// and two MCU pins. When the connectivity is okay, one of the two pins outputs a different level,
// and the other pin can read a different level. The input level of the fifth group is opposite to
// the output level, and the rest are the same.
// T-Sensor was originally visited on SACP, but there is no SACP in the PCBA stage of the production line.
// For this reason, the production line will connect the I2C of the MCU and the I2C of the T-Sensor (and EEPROM)
// through a fixture, so the T-Sensor can also be tested. In this case, 0-60 degrees Celsius is considered the normal range.
#define DB9_CONNECTIVITY_PAIR_COUNT 5

// 5 output pins
static const struct mcu_pin_t connectvity_out_pin_tab[DB9_CONNECTIVITY_PAIR_COUNT] = {
    [CONTIVITY_TEST_RESULT_IDX_1            ] = { GPIOA, GPIO_PIN_15, GPIO_MODE_OUT_PP      },
    [CONTIVITY_TEST_RESULT_IDX_2            ] = { GPIOB, GPIO_PIN_13, GPIO_MODE_OUT_PP      },
    [CONTIVITY_TEST_RESULT_IDX_3            ] = { GPIOB, GPIO_PIN_6 , GPIO_MODE_OUT_PP      },
    [CONTIVITY_TEST_RESULT_IDX_I2C          ] = { GPIOB, GPIO_PIN_7 , GPIO_MODE_OUT_PP      },
    [CONTIVITY_TEST_RESULT_IDX_OPTO_ISOLATOR] = { GPIOB, GPIO_PIN_1 , GPIO_MODE_OUT_PP      },
};

// 5 input pins
static const struct mcu_pin_t connectvity_in_pin_tab[DB9_CONNECTIVITY_PAIR_COUNT] = {
    [CONTIVITY_TEST_RESULT_IDX_1            ] = { GPIOB, GPIO_PIN_4 , GPIO_MODE_IPD         },
    [CONTIVITY_TEST_RESULT_IDX_2            ] = { GPIOB, GPIO_PIN_3 , GPIO_MODE_IPD         },
    [CONTIVITY_TEST_RESULT_IDX_3            ] = { GPIOB, GPIO_PIN_8 , GPIO_MODE_IPD         },
    [CONTIVITY_TEST_RESULT_IDX_I2C          ] = { GPIOB, GPIO_PIN_5 , GPIO_MODE_IPD         },
    [CONTIVITY_TEST_RESULT_IDX_OPTO_ISOLATOR] = { GPIOB, GPIO_PIN_9 , GPIO_MODE_IN_FLOATING },
};

#ifdef __FACTORY_RELEASE__
static const struct mcu_pin_t opto_isolator_in_factory =
    { GPIOB, GPIO_PIN_9 , GPIO_MODE_IPD };
#endif

// only #5 are reverted
static inline bool is_connectvity_revert(uint8_t idx)
{
    return idx == CONTIVITY_TEST_RESULT_IDX_OPTO_ISOLATOR; // OPTO_ISOLATOR pair is revert, others are not
}


// On the production line, the DB9 test board may be plugged and unplugged under power,
// so try to keep its pins low to reduce the defect rate.
// But outside the production line, it is better to keep these pins open-drain input.
// a macro __FACTORY_RELEASE__ should be defined for factory release,
// otherwise the function do nothing
// parameter and value: none
void db9_init_for_factory(void)
{
    // power on the pins
    enable_rcus(rpu_tab, sizeof(rpu_tab) / sizeof(rpu_tab[0]));
    // pin should be remapped because it used by connectivity test
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

#ifdef __FACTORY_RELEASE__
    // pins' mode setup
    setup_pins(connectvity_out_pin_tab, DB9_CONNECTIVITY_PAIR_COUNT);
    setup_pins(connectvity_in_pin_tab , DB9_CONNECTIVITY_PAIR_COUNT);

    setup_pins(&opto_isolator_in_factory, 1);

    write_pin(connectvity_out_pin_tab + CONTIVITY_TEST_RESULT_IDX_1, false);
    write_pin(&opto_isolator_in_factory, true);
#endif
}


// test BD9 connectivity
// parameter:
// [out] db9_test_result, ptr to echo test value of DB9. 0/1 for false/true
// return value: none
static void test_db9_connectivity(uint32_t *db9_test_result)
{
    // power on the pins
    enable_rcus(rpu_tab, sizeof(rpu_tab) / sizeof(rpu_tab[0]));
    // pin should be remapped because it used by connectivity test
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    // pins' mode setup
    setup_pins(connectvity_out_pin_tab, DB9_CONNECTIVITY_PAIR_COUNT);
    setup_pins(connectvity_in_pin_tab , DB9_CONNECTIVITY_PAIR_COUNT);


    for (int i = 0; i < DB9_CONNECTIVITY_PAIR_COUNT; i++) {
        db9_test_result[i] = true;
    }

    // test 2 level of pin
    for (int v = 0; v < 2; v++) {

        const bool out_value = !(bool)v;
        // for each pin pair, write-out
        for (int i = 0; i < DB9_CONNECTIVITY_PAIR_COUNT; i++) {
            // boolean of level writting
            write_pin(connectvity_out_pin_tab + i, out_value);
        }

        // wait circuit's voltage be stable
        osDelay(120);

        // for each pin pair read-back
        for (int i = 0; i < DB9_CONNECTIVITY_PAIR_COUNT; i++) {
            // is it a revert pair?
            const bool revert_flag = is_connectvity_revert(i);
            // the input pin should read this
            const bool expect_in_value = out_value ^ revert_flag;
            // read it
            const bool in_value = read_pin(connectvity_in_pin_tab + i);

            // twice should be both OK
            db9_test_result[i] &= in_value == expect_in_value;
        }
    }
}


// ReplyToConnectivityTest, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. Because the connectivity test is executed
// under specific circumstances, it does not need to be placed in a separate thread, but directly
// in the receiving thread. In this case, it is not a problem that the receiving thread is
// blocked for hundreds of milliseconds. See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [out] test_item_list, the test result 0/1 (false/true) array for each test item.
//   [in]  seq, the request seq
void ReplyToConnectivityTest(res_error_code_t *error_code, uint32_t *test_item_list, serial_datagram_item_t seq)
{
    (void)seq;
    *error_code = NO_ERROR;

    // the first five test items are about DB9.
    test_db9_connectivity(test_item_list);

    // T-Sensor value test
    temperature_t temp = 0;
    bool isOK = get_temperature(&temp);
    isOK = isOK && (temperature_in_c_degree(temp) > 0 && temperature_in_c_degree(temp) <= 60); // is temperature in the normal range?
    // a trick to output human-readable temperature
    rpc_log(LOG_INFO, "temperature: %s%03d.%04d", temp >= 0 ? "+" : "-", abs(temp) >> 8, (abs(temp) & 0xF0) * 10000 / 256);
    // save the value
    test_item_list[CONTIVITY_TEST_RESULT_IDX_T_SENSOR] = isOK;

    db9_init_for_factory();
}
