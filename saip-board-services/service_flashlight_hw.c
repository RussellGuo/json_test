/*
 * service_flashlight_hw.c
 *
 * hardware-depended part of flashlight service for the saip board
 *
 *  Created on: Dec 11, 2020
 *      Author: Guo Qiang
 */
 
#if defined(SAIP_BOARD) // same name file for different board

#include "service_flashlight.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"
#include "mcu-common.h"

// The hardware initialization function required for the operation of the flashlight module.
// TODO: implement it
void flashlight_hw_init(void)
{
    // temporarily use this function as a debugging tool
    // let the UART output something for debugging the communication part of the host
    for(int i;; i++) {
        osDelay(1000);
        rpc_log(LOG_WARN, "idle %d", i);
    }
}


// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will always wait
// parameters:
// [in] flashlight_mode, flashlight_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the flashlight mode and/or mode parameter,
//    the flag of this thread will be set. This is a notice to change mode.
// TODO: implement it
void flashlight_running_until_new_request_arrived(uint32_t flashlight_mode, uint32_t flashlight_mode_param, uint32_t new_request_flag)
{
    wait_flag(new_request_flag);
}

#endif
