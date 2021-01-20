/*
 * service_camera_hw.c
 *
 * hardware-depended part of camera service for the SAIP
 * This service does not need a separate thread, because the response operation is very fast
 * and will not block the receiving thread of the protocol.
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 * for the configuration method.
 *
 *  Created on: Jan 6, 2021
 *      Author: Guo Qiang
 */

#include "semantic_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"
#include "mcu-common.h"

#include "mcu-camera-mode.h"

static void set_camera(uint32_t mode)
{
    // TODO: setup hardware register, GPIO, PWM, etc.
}


// ReplyToSetCameraConfig, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in]  seq, the request seq
void ReplyToSetCameraConfig(serial_datagram_item_t mode, res_error_code_t *error_code, serial_datagram_item_t seq)
{
    (void)seq;
    *error_code = ERR_MODE;

    rpc_log(LOG_DEBUG, "ReplyToSetCameraConfig(%u)", mode);
    switch(mode) {
    case CAMERA_MODE_PRIMARY:
    case CAMERA_MODE_SECONDARY:
        *error_code = NO_ERROR;
        set_camera(mode);
        break;

    }
}
