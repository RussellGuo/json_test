/*
 * service_flashlight.h
 *
 * Implementation of MCU flashlight service module
 * MCU flashlight service module, this module will run autonomously, SaCP can configure
 *  the mode and mode parameters at any time. Refer to the documentation
 * "SaIP communication protocol between the host and the MCU" for the configuration method.
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#ifndef __SERVICE_FLASHLIGHT_H__
#define __SERVICE_FLASHLIGHT_H__

#include <stdint.h>
#include <stdbool.h>

// API part

// start the flashlight service, should be invoked at the beginning.
// Return value:
//   true means success, otherwise failure
bool init_thread_of_service_flashlight(void);


// Inner interface part.
// Those functions are hardware-depended, and SaIP and start-board need to be implemented
//  separately (the implementation of start-board is only used in the development process).

// The hardware initialization function required for the operation of the flash light module.
void flashlight_hw_init(void);

// Perform the service according to the specified mode and mode parameter,
// and return immediately if a new request is received during the period,
// otherwise it will loop indefinitely
// parameters:
// [in] flashlight_mode, flashlight_mode_param as the protocol
// [in] new_request_flag: when the underlying protocol modifies the flash light mode and/or mode parameter,
//    the flag of this thread will be set. This is an notice to change mode.
void flashlight_running_until_new_request_arrived(uint32_t flashlight_mode, uint32_t flashlight_mode_param, uint32_t new_request_flag);

#endif
