/*
 * i2c-base.c
 *
 * common routines for I2C devices, such as flashlight, T-Sensor, EEPROM
 * Note: T-Sensor and EEPROM can be accessed only in factory test mode
 *
 *  Created on: Jan 15, 2021
 *      Author: Guo Qiang
 */

#include "i2c-base.h"

#include "gd32e10x.h"
#include "mcu-hw-common.h"

#include "cmsis_os2.h"

//Each I2C bus is exclusive, and exclusive resources should have a mutex.
#define I2C_BUS_COUNT 2
static osMutexId_t mutex_i2c_bus_id[I2C_BUS_COUNT];
static const osMutexAttr_t mutex_i2c_bus_attr[I2C_BUS_COUNT] = {
    {
        .name = "i2c0_bus_mutex",                          // human readable mutex name
        .attr_bits = osMutexRecursive | osMutexPrioInherit,    // attr_bits
    },
    {
        .name = "i2c1_bus_mutex",                          // human readable mutex name
        .attr_bits = osMutexRecursive | osMutexPrioInherit,    // attr_bits
    },
};


// return the mutex of the I2C
// parameter:
//   [in] i2c_no, should be I2C0 or I2C1
// return value: the mutex id if OK, NULL otherwise
static inline osMutexId_t mutex_for_i2c(uint32_t i2c_no)
{
    switch(i2c_no) {
    case I2C0:
        return mutex_i2c_bus_id[0];

    case I2C1:
        return mutex_i2c_bus_id[1];

    default:
        return NULL;
    }
}

// make initial preparations for I2C bus access
// it should be invoked at an earlier time, for example in main()
// parameter: none
// return value: true means success, otherwise failure
bool init_i2c_buses(void)
{
    bool ret = true;

    // create a mutex for each I2C bus
    for (int i = 0; i < I2C_BUS_COUNT; i++) {
        mutex_i2c_bus_id[i] = osMutexNew (&mutex_i2c_bus_attr[i]);
        ret = ret && (mutex_i2c_bus_id[i] != NULL);
    }

    return ret;
}


// i2c power control. I2C0 should enable those RCUs
static rcu_periph_enum i2c0_rpu_tab[] = {
    RCU_GPIOB,
    RCU_I2C0,
};

// i2c power control. I2C1 should enable those RCUs
static rcu_periph_enum i2c1_rpu_tab[] = {
    RCU_GPIOB,
    RCU_I2C1,
};

#define I2C0_SPEED 100000

// i2c0 GPIO PIN definition
static const struct mcu_pin_t i2c0_pin_tab[] = {
    { GPIOB, GPIO_PIN_6 , GPIO_MODE_AF_OD }, // clk
    { GPIOB, GPIO_PIN_7 , GPIO_MODE_AF_OD }, // data
};

// i2c1 GPIO PIN definition
static const struct mcu_pin_t i2c1_pin_tab[] = {
    { GPIOB, GPIO_PIN_10, GPIO_MODE_AF_OD }, // clk
    { GPIOB, GPIO_PIN_11, GPIO_MODE_AF_OD }, // data
};

// occupy an I2C bus and init it. i2c_hw_init() needs to be invoked
// at the beginning of the session of MCU and device, and i2c_hw_deinit()
// needs to be invoked at the end.
// parameter:
//   [in] i2c_no, should be I2C0 or I2C1
// return value: true means success, otherwise failure (mostly, another thread is also using this I2C)
bool i2c_hw_init(uint32_t i2c_no)
{
    // occupy the bus (software domain)
    osMutexId_t mutex_i2c_id = NULL;             // bus mutex
    mutex_i2c_id = mutex_for_i2c(i2c_no);
    if (osMutexAcquire(mutex_i2c_id, 0) != osOK) {
        return false;
    }

    const struct mcu_pin_t *i2c_pin_tab = NULL;  // PINs
    size_t i2c_pin_tab_len = 0;
    const rcu_periph_enum *i2c_rpu_tab = NULL;   // RPUs
    size_t i2c_rpu_tab_len = 0;


    // find PINs/RPUs of corresponding I2C bus
    switch(i2c_no) {
    case I2C0:
        i2c_pin_tab = i2c0_pin_tab;
        i2c_pin_tab_len = sizeof(i2c0_pin_tab) / sizeof(i2c0_pin_tab[0]);
        i2c_rpu_tab = i2c0_rpu_tab;
        i2c_rpu_tab_len = sizeof(i2c0_rpu_tab) / sizeof(i2c0_rpu_tab[0]);

        break;

    case I2C1:
        i2c_pin_tab = i2c1_pin_tab;
        i2c_pin_tab_len = sizeof(i2c1_pin_tab) / sizeof(i2c1_pin_tab[0]);
        i2c_rpu_tab = i2c1_rpu_tab;
        i2c_rpu_tab_len = sizeof(i2c1_rpu_tab) / sizeof(i2c1_rpu_tab[0]);

        break;

    default:
        return false;
    }

    // init PINs and RPUs
    setup_pins(i2c_pin_tab, i2c_pin_tab_len);
    enable_rcus(i2c_rpu_tab, i2c_rpu_tab_len);

    // init bus registers
    i2c_clock_config(i2c_no, I2C0_SPEED, I2C_DTCY_2); // configure I2C clock
    i2c_enable(i2c_no);                               // enable I2C
    i2c_ack_config(i2c_no, I2C_ACK_ENABLE);           // enable acknowledge

    return true;
}

// deinit an I2C bus and release it. i2c_hw_init() needs to be invoked
// at the beginning of the session of MCU and device, and i2c_hw_deinit()
// needs to be invoked at the end.
// parameter:
//   [in] i2c_no, should be I2C0 or I2C1
// return value: true means success, otherwise failure
bool i2c_hw_deinit(uint32_t i2c_no)
{
    // deinit the I2C
    i2c_ack_config(i2c_no, I2C_ACK_DISABLE);     // disable acknowledge
    i2c_stop_on_bus(i2c_no);                     // stop the bus
    i2c_deinit(i2c_no);                          // reset the I2C controller

   // release bus (software domain)
    osMutexId_t mutex_i2c_id = NULL;             // bus mutex
    mutex_i2c_id = mutex_for_i2c(i2c_no);
    return osMutexRelease(mutex_i2c_id) == osOK;
}


// wait an I2C flag be set in 1ms. it's a busy loop waitting
// it will be invoked repeatedly during the MCU and device session
// parameters:
//   [in] i2c_no, should be I2C0 or I2C1
//   [in] flag, such as I2C_FLAG_SBSEND, I2C_FLAG_ADDSEND, I2C_FLAG_TBE, I2C_FLAG_RBNE
// return value: true means flag is set, otherwise timeout
bool wait_for_i2c_flag(uint32_t i2c, i2c_flag_enum flag)
{
    uint32_t timeout_in_ms = 1; // 1ms
    uint32_t begin_time  = osKernelGetSysTimerCount();
    // 1us is short interval then the value is not overflow.
    uint32_t timeout_in_systick = timeout_in_ms * (osKernelGetSysTimerFreq() / 1000u); // systick count for delay
    while (true) {
        FlagStatus flagStatus = i2c_flag_get(i2c, flag);
        if (flagStatus == SET) {
            return true;
        }

        // it's trick, that timerCount() is always overflow, then use 'current_time - begin_time > diff' will be OK,
        // but 'current_time > begin_time + diff' will be not.
        uint32_t current_time = osKernelGetSysTimerCount();
        if ((current_time - begin_time) > timeout_in_systick) {
            return false;
        }
    }
}
