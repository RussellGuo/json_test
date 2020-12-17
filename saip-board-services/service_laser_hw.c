/*
 * service_laser_hw.c
 *
 *
 * hardware-depended part of laser service for saip board
 *
 *  Created on: Dec 12, 2020
 *      Author: Guo Qiang
 */

#if defined(SAIP_BOARD) // same name file for different board

#include "service_laser.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"
#include "mcu-common.h"

// The laser needs 10kHz PWM with a 1:1 duty cycle
//TIMER1CLK = SystemCoreClock / 120 = 1MHz, the PWM frequency is 10kHz.
//TIMER1 channel1 duty cycle = (50 / 100)* 100  = 50%
#define LASER_PWM                TIMER1
#define LASER_COUNT                   3
#define LASER_PRESCALER             120
#define LASER_PERIOD                100
#define LASER_REPETITIONCOUNTER       0
#define LASER_DUTY                   50

static void pwm_config(void)
{
    timer_parameter_struct timer_initpara;
    timer_struct_para_init(&timer_initpara);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = LASER_PRESCALER - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = LASER_PERIOD;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = LASER_REPETITIONCOUNTER;

    timer_init(TIMER1, &timer_initpara);


    timer_oc_parameter_struct timer_ocinitpara;
    timer_channel_output_struct_para_init(&timer_ocinitpara);

    /* initialize TIMER channel output parameter struct */
    /* CH1 configuration in PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(LASER_PWM, TIMER_CH_1, &timer_ocinitpara);


    /* CH1 configuration in PWM mode0, duty cycle 50% */
    timer_channel_output_pulse_value_config(LASER_PWM, TIMER_CH_1,   LASER_DUTY           );
    timer_channel_output_mode_config       (LASER_PWM, TIMER_CH_1, TIMER_OC_MODE_PWM0     );
    timer_channel_output_shadow_config     (LASER_PWM, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(LASER_PWM);
}


// LED GPIO power control
static rcu_periph_enum rpu_tab[] = {
    RCU_GPIOA,   // laser enable pin, output
    RCU_GPIOB,   // laser status pin, input
    RCU_AF,      // alternate function
    RCU_TIMER1,  // PWM for laser
};


// LASER GPIO PIN definition
static const struct {
    uint32_t pin_port;
    uint32_t pin_no;
} laser_enable_pin = {GPIOB, GPIO_PIN_14}, // PIN B14
  laser_status_pin = {GPIOA, GPIO_PIN_0 }, // PIN  A0
  laser_pwm_pin    = {GPIOA, GPIO_PIN_1} ; // PIN  A1

// The hardware initialization function required for the operation of the laser module.
void laser_hw_init(void)
{
    /* configure RCU of laser GPIO port */
    for (int i = 0; i < sizeof(rpu_tab) / sizeof(rpu_tab[0]); i++) {
        rcu_periph_clock_enable(rpu_tab[i]);
    }

    /* configure Laser GPIO port */
    gpio_init(laser_enable_pin.pin_port, GPIO_MODE_OUT_PP     , GPIO_OSPEED_50MHZ, laser_enable_pin.pin_no);
    gpio_init(laser_status_pin.pin_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, laser_status_pin.pin_no);
    gpio_init(laser_pwm_pin   .pin_port, GPIO_MODE_AF_PP      , GPIO_OSPEED_50MHZ, laser_pwm_pin.pin_no   );

    pwm_config();
}

// turn on/off of the laser
// parameters:
//  [in]on, enable/disable the laser
// return value:
//  true means success, otherwise failure
// TODO: implement it
static bool turn_laser(bool on)
{
    rpc_log(LOG_VERBOSE, "turn_laser '%s'", on ? "on" : "off");

    // enable/disable the timer according parameter 'on'
    (on ? timer_enable : timer_disable)(LASER_PWM);
    // set the enable pin
    gpio_bit_write(laser_enable_pin.pin_port, laser_enable_pin.pin_no, on ? SET : RESET);

    return true;
}

// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will always wait
// parameters:
// [in] laser_mode, laser_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the laser mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.
void laser_running_until_new_request_arrived(uint32_t laser_mode, uint32_t laser_mode_param, uint32_t new_request_flag)
{
    if (laser_mode == MCU_MODULE_OFF) {
        // OFF mode
        turn_laser(false);
    }

    if (laser_mode == LASER_MODE_FACTORY_TEST) {
        // ON mode
        turn_laser(true);
    }

    wait_flag(new_request_flag);
}

#endif
