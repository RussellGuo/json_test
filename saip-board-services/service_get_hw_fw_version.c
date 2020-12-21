/*
 * service_get_hw_fw_version.c
 *
 * Implementation of 'get MCU's hardware version and firmware version'
 * This service does not need a separate thread, because the response operation is very fast
 * and will not block the receiving thread of the protocol.
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 * for the configuration method.
 *
 *  Created on: Dec 21, 2020
 *      Author: Guo Qiang
 */

#include "semantic_api.h"

// version information is palced in the starup code, pls see "startup_gd32e10x.s" for the reason.
// the vesion number are 2 uint32_t values
#define __SAIP_HW_VERSION_IDX 0
#define __SAIP_FW_VERSION_IDX 1
extern const uint32_t __SAIP_MCU_VERSION[2];


// ReplyToSetMcuLogLevel, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose.  See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [out] HwVersion, FwVersion, the ptr to the return values
//   [in]  seq, the request seq
void ReplyToReqHwFwVersion(res_error_code_t *error_code, uint32_t *HwVersion, uint32_t *FwVersion, serial_datagram_item_t seq)
{
    *error_code = NO_ERROR;
    (void)seq;

    *HwVersion = __SAIP_MCU_VERSION[__SAIP_HW_VERSION_IDX];
    *FwVersion = __SAIP_MCU_VERSION[__SAIP_FW_VERSION_IDX];
}

