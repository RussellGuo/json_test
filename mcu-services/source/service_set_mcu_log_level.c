/*
 * service_set_mcu_log_level.c
 *
 * Implementation of set MCU log level from host
 * This service does not need a separate thread, because the response operation is very fast
 * and will not block the receiving thread of the protocol.
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 * for the configuration method.
 *
 *  Created on: Dec 19, 2020
 *      Author: Guo Qiang
 */

#include "service_led.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"

// ReplyToSetMcuLogLevel, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose.  See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in]  log_level, as its name. value domain is log_level_t
//   [in]  seq, the request seq
void ReplyToSetMcuLogLevel(res_error_code_t *error_code, serial_datagram_item_t log_level, serial_datagram_item_t seq)
{
    // clamp log level
    if (log_level > LOG_FATAL || log_level < LOG_VERBOSE) {
        *error_code = ERR_PARAM;
    } else {
        *error_code = NO_ERROR;
        set_rpc_log_level((log_level_t) log_level);
    }
    (void)seq;
}
