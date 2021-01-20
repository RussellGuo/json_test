/*
 * service_get_run_info.c
 *
 * Implementation of 'get MCU's run info of the protocol'
 * This service does not need a separate thread, because the response operation is very fast
 * and will not block the receiving thread of the protocol.
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 * for the configuration method.
 *
 *  Created on: Dec 24, 2020
 *      Author: Guo Qiang
 */

#include "semantic_api.h"
#include "run_info_result_desc.h"

#include "cmsis_os2.h"

#include <string.h>

// ReplyToRunInfo, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose.  See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [out] run_info_list, the ptr to the return values, the 'protocol run info'
//   [in]  seq, the request seq

void ReplyToRunInfo(res_error_code_t *error_code, uint32_t *run_info_list, serial_datagram_item_t seq)
{
    // get the run info array
    const serial_datagram_item_t *tab = get_local_run_info_tab();
    // copy them to the host
    memcpy(run_info_list, tab, sizeof(run_info_list[0]) * RUN_INFO_RESULT_COUNT);
    run_info_list[RUN_INFO_IDX_TICK_COUNT] = osKernelGetTickCount();
    *error_code = NO_ERROR;
    (void)seq;
}
