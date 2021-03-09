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

#include "service_camera.h"

#include "semantic_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"
#include "mcu-common.h"

#include "mcu-camera-mode.h"

#include "mcu-hw-common.h"

// Camera mode GPIO PIN power control
static const rcu_periph_enum rpu_tab[] = {
    RCU_GPIOA,
};

// Camera mode GPIO PIN definition
static const struct mcu_pin_t camera_mode_pin = { GPIOA, GPIO_PIN_11 , GPIO_MODE_OUT_PP};
static const struct mcu_pin_t camera_clr_pin  = { GPIOA, GPIO_PIN_12 , GPIO_MODE_OUT_PP};

static void set_camera_mode(uint32_t mode)
{
    /* configure RCU of Camera mode GPIO port */
    enable_rcus(rpu_tab, sizeof(rpu_tab) / sizeof(rpu_tab[0]));
    /* configure Camera mode GPIO port */
    setup_pins(&camera_mode_pin, 1);
    setup_pins(&camera_clr_pin , 1);

    switch(mode) {
    case CAMERA_MODE_PRIMARY:
    case CAMERA_MODE_SECONDARY:
        write_pin(&camera_mode_pin, mode);
        write_pin(&camera_clr_pin , true);
    }

}


// The hardware initialization function required for the operation of the camera module.
void camera_hw_init(void)
{
    set_camera_mode(CAMERA_MODE_PRIMARY);
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
        set_camera_mode(mode);
        break;

    }
}

