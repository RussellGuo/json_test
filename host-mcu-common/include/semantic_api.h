/*
 * semantic_layer.h
 *
 * Implementation of Semantic Layer of Host-MCU Communication Protocol
 *
 *  Created on: Dec 2, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */

#ifndef __SEMANTIC_LAYER_H_
#define __SEMANTIC_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "serial_datagram.h"


// For a request, possible return error code
typedef enum {
    NO_ERROR        =  0,
    ERR_CRC         =  1,   // crc check error
    ERR_MSG_ID      =  2,   // the request code not found
    ERR_ITEM_COUNT  =  3,   // wrong number of parameters or return list
    ERR_NO_IMPL     =  4,   // The message format is correct, but the implementation is not ready
    ERR_NOT_READY   =  5,   // the module not ready. i.e. thread create failed
    ERR_MODE        =  6,   // mode error at the server execution
    ERR_PARAM       =  7,   // parameter error at the server execution
    ERR_UNKNOWN     = 15,   // unknown error at the server execution
} res_error_code_t;


// mode for the LED, Laser and the Flash light
#define MCU_MODULE_OFF 0


#define LASER_MODE_USER1                0x11
#define LASER_MODE_USER2                0x12
#define LASER_MODE_FACTORY_TEST         0x18

#define FLASHLIGHT_MODE_USER1           0x21
#define FLASHLIGHT_MODE_USER2           0x22
#define FLASHLIGHT_MODE_FACTORY_TEST    0x28



// the msg id, request id mostly
typedef enum {
    REQ_HW_FW_VERSION      =  8,
    REQ_RUN_INFO           =  9,
    SET_LED_CONFIG         = 10,
    SET_LASER_CONFIG       = 11,
    SET_FLASHLIGHT_CONFIG  = 12,
    START_FACTORY_TEST     = 13,
} msg_id_t;

// MCU is the SERVER
#if defined(IS_MCU_SIDE)
#define PROTOCOL_SERVER_SIDE 1
#else
#define PROTOCOL_SERVER_SIDE 0
#endif

// Semantic Layer API
// The complete action of a semantic layer is started from the host side,
// reaches the execution part of the MCU, and finally returns to the host side.
// This description deliberately hides the interactive process of the datagram
// layer, presenting a simple and necessary interface to the user.

// This series of functions, 'ReqHwFwVersion', is the interface to "started from the host side".
// The 'ReplyToReqHwFwVersion' series of functions are the interface to the "execution part of the MCU".
// After "return to host side", the 'DispatchReplyOfReqHwFwVersion' series functions on the host will be invoked.

// The 'ReqHwFwVersion' series of functions are implemented in this semantic layer, and users only need to invoke them.
// the 'ReplyToReqHwFwVersion' series of functions need to be finalized in various related modules on the MCU side.
// This semantic layer provides an implementation of the 'WEAK' version, which is to test the protocol implementation
// before the final implementation. After the final version is completed, the "WEAK" version does not need to be deleted,
// and the linker will automatically select the version other than "WEAK".
// the 'DispatchReplyOfReqHwFwVersion' series of functions that need to be finalized in various related modules on the Host side.
// This semantic layer provides an implementation of the 'WEAK' version. The situation of the 'WEAK' version is the same as
// the situation in the previous paragraph.

#if PROTOCOL_SERVER_SIDE == 0

// start an action from the host side. Return true means success, otherwise failure
// The seq is an ID, and the same seq will be returned in the DispatchReplyOfXXX of the same action.
// Their parameters are consistent with the documentation. Please refer to the documentation
// "SaIP communication protocol between the host and the MCU"
bool ReqHwFwVersion(serial_datagram_item_t seq);
bool ReqRunInfo(serial_datagram_item_t seq);
bool SetLedConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq);
bool SetLaserConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq);
bool SetFlashlightConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq);
bool StartFactoryTest(serial_datagram_item_t seq);

// A server accessing through a long call chain, and finally reaches the following functions.
// Every parameter is [in]. the values other than seq comes from 'ReplyToXXX'
void DispatchReplyOfReqHwFwVersion(const res_error_code_t error_code, const uint32_t HwVersion, const uint32_t FwVersion, serial_datagram_item_t seq);
void DispatchReplyOfRunInfo(const res_error_code_t error_code, const uint32_t *run_info_list, serial_datagram_item_t seq);
void DispatchReplyOfSetLedConfig(const res_error_code_t error_code, serial_datagram_item_t seq);
void DispatchReplyOfSetLaserConfig(const res_error_code_t error_code, serial_datagram_item_t seq);
void DispatchReplyOfSetFlashlightConfig(const res_error_code_t error_code, serial_datagram_item_t seq);
void DispatchReplyOfStartFactoryTest(const res_error_code_t error_code, const uint32_t *test_item_list, serial_datagram_item_t seq);

#else

// The interface to the "execution part of the MCU". The ReplyToReqHwFwVersion series of functions are the
// actual MCU execution functions: parameters are passed in, and the return value and error code are obtained.
// [out]error_code: the pointer of the error code.
// [out]every pointer without const qualifier is [out]
// most parameters are passed from the 'ReqHwFwVersion' likes function,
//  and [out]values will be returned to 'DispatchReplyOfXXX'
void ReplyToReqHwFwVersion(res_error_code_t *error_code, uint32_t *HwVersion, uint32_t *FwVersion, serial_datagram_item_t seq);
void ReplyToRunInfo(res_error_code_t *error_code, uint32_t *run_info_list, serial_datagram_item_t seq);
void ReplyToSetLedConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, res_error_code_t *error_code, serial_datagram_item_t seq);
void ReplyToSetLaserConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, res_error_code_t *error_code, serial_datagram_item_t seq);
void ReplyToSetFlashlightConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, res_error_code_t *error_code, serial_datagram_item_t seq);
void ReplyToStartFactoryTest(res_error_code_t *error_code, uint32_t *test_item_list, serial_datagram_item_t seq);

#endif


#ifdef __cplusplus
}
#endif

#endif /* __SEMANTIC_LAYER_H_ */
