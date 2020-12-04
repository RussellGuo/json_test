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
    NO_ERROR        = 0,
    ERR_CRC         = 1,   // crc check error
    ERR_MSG_ID      = 2,   // the request code not found
    ERR_ITEM_COUNT  = 3,   // wrong number of parameters or return list 
    ERR_EXEC        = 4,   // The message format is correct, but the execution is wrong after the dispatching
} res_error_code_t;

// the msg id, request id mostly
typedef enum {
    REQ_HW_FW_VERSION      =  8,
    REQ_RUN_INFO           =  9,
    SET_LED_CONFIG         = 10,
    SET_LASER_CONFIG       = 11,
    SET_FLASH_LIGHT_CONFIG = 12,
    START_FACTORY_TEST     = 13,
} msg_id_t;

// MCU is the remote calling SERVER
#if defined(IS_MCU_SIDE)
#define PROTOCOL_SERVER_SIDE 1
#else
#define PROTOCOL_SERVER_SIDE 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SEMANTIC_LAYER_H_ */
