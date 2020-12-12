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


// The hardware initialization function required for the operation of the laser module.

// TODO: implement it
void laser_hw_init(void)
{
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
    return true;
}

// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will always wait
// parameters:
// [in] laser_mode, laser_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the laser mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.

// TODO: implement it
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
