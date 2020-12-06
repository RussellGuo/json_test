/*
 * semantic_layer.h
 *
 * Implementation of Semantic Layer of Host-MCU Communication Protocol
 *
 *  Created on: Dec 2, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */

#include "semantic_api.h"
#include <string.h>

// Regardless of the host side or the MCU side, the data delivered by the opposite end is 
// accessed through the datagram layer. The data at this point is of general uint32_t type.
// That general data is suitable for processing by looking up the table.
// At the end of the processing, a specific, strongly typed function(which is the 'ReplyToXXX' 
//  at the MCU side, or 'DispatchReplyOfXXX' at the host side)is invoking through a short function.
// The following tables are for MCU side and Host side both. Table looking up and processing are
// slightly different for the 2 sides.

#if PROTOCOL_SERVER_SIDE

// generic processing function protocol definition for the MCU side
#define MSG_PROC_PROTOTYPE \
    const serial_datagram_item_t input_item_list[], serial_datagram_item_t output_item_list[], const serial_datagram_item_t seq
#define MSG_PROC_RET_TYPE res_error_code_t

#else

// generic processing function definition for the host side
#define MSG_PROC_PROTOTYPE \
   const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq
#define MSG_PROC_RET_TYPE void
#endif

// generic processing functions
static MSG_PROC_RET_TYPE      req_hw_fw_version_msg_proc(MSG_PROC_PROTOTYPE);
static MSG_PROC_RET_TYPE           req_run_info_msg_proc(MSG_PROC_PROTOTYPE);
static MSG_PROC_RET_TYPE         set_led_config_msg_proc(MSG_PROC_PROTOTYPE);
static MSG_PROC_RET_TYPE       set_laser_config_msg_proc(MSG_PROC_PROTOTYPE);
static MSG_PROC_RET_TYPE set_flash_light_config_msg_proc(MSG_PROC_PROTOTYPE);
static MSG_PROC_RET_TYPE     start_factory_test_msg_proc(MSG_PROC_PROTOTYPE);


typedef MSG_PROC_RET_TYPE (*semantic_msg_process_t)(MSG_PROC_PROTOTYPE);

typedef struct {
    msg_id_t msg_id;                    // as its name
    bool     direction_to_mcu;          // now it's always true
    uint8_t  req_len;                   // length of parameters
    uint8_t  res_len;                   // length of return values
    semantic_msg_process_t msg_proc;    // generic procssing func
} sematic_layer_info_t;

static const sematic_layer_info_t *get_sematic_layer_info(serial_datagram_item_t msg_id);

// Here is the table
static const sematic_layer_info_t sematic_layer_info_tab[] = {
    { REQ_HW_FW_VERSION     , true , 0,  2, req_hw_fw_version_msg_proc      },
    { REQ_RUN_INFO          , true , 0,  4, req_run_info_msg_proc           },
    { SET_LED_CONFIG        , true , 2,  1, set_led_config_msg_proc         },
    { SET_LASER_CONFIG      , true , 2,  1, set_laser_config_msg_proc       },
    { SET_FLASH_LIGHT_CONFIG, true , 2,  1, set_flash_light_config_msg_proc },
    { START_FACTORY_TEST    , true,  0, 10, start_factory_test_msg_proc     },
};

// find the info by msg_id
// [in]msg_id, the id
// return a pointer to the table if found, or else NULL
static const sematic_layer_info_t *get_sematic_layer_info(serial_datagram_item_t msg_id)
{
    for (size_t i = 0; i < sizeof sematic_layer_info_tab / sizeof (sematic_layer_info_tab[0]); i++) {
        if (sematic_layer_info_tab[i].msg_id == msg_id) {
            return sematic_layer_info_tab + i;
        }
    }
    return 0;
}

#if PROTOCOL_SERVER_SIDE

// MCU side datagram processing

// Once a datagram be received from remote, this function will be invoked
// Parameters:
//   [in]seq, msg_id, data list and data count
void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len)
{
    res_error_code_t error_code = NO_ERROR;

    // lookup the table
    const sematic_layer_info_t *const sematic_layer_info = get_sematic_layer_info(msg_id);

    // Check the msg id
    if (sematic_layer_info == 0 || !sematic_layer_info->direction_to_mcu) {
        error_code = ERR_MSG_ID;
    }

    // check parameter length
    if (error_code == NO_ERROR) {
        if (sematic_layer_info->req_len != len) {
            error_code = ERR_ITEM_COUNT;
        }
    }

    // invoke the generic processing function
    serial_datagram_item_t res_items[MAX_DATAGRAM_ITEM_COUNT] = {0};
    if (error_code == NO_ERROR) {
        error_code = sematic_layer_info->msg_proc(data_list, res_items + 1, seq);
    }

    // reply the response
    size_t res_item_len = sematic_layer_info->res_len;
    res_items[0] = error_code;
    serial_datagram_send(seq, msg_id, res_items, res_item_len + 1);
}

#else

// Once a datagram be received from remote, this function will be invoked
// Parameters:
//   [in]seq, msg_id, data list and data count
void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len)
{
    // lookup the table
    const sematic_layer_info_t *const sematic_layer_info = get_sematic_layer_info(msg_id);
    if (sematic_layer_info == 0) {
        // log an ERR_MSG_ID
        return;
    }

    // // check return value length
    if (sematic_layer_info->res_len + 1 != len) {
        // log an item count error
        return;
    }

    // invoke the generic processing function
    sematic_layer_info->msg_proc(data_list + 1, (res_error_code_t)data_list[0], seq);
}

#endif

#if PROTOCOL_SERVER_SIDE


// a stub of function 'ReplyToReqHwFwVersion'
__attribute__((weak)) void ReplyToReqHwFwVersion(
    res_error_code_t *error_code,
    uint32_t *HwVersion, uint32_t *FwVersion,
    serial_datagram_item_t seq)
{
    *HwVersion = 0x55AA; *FwVersion = 0xAA55; // test value
    *error_code = ERR_NO_IMPL;
    (void)seq;
}

// a stub of function 'ReplyToRunInfo'
__attribute__((weak)) void ReplyToRunInfo(
    res_error_code_t *error_code,
    uint32_t *run_info_list, serial_datagram_item_t seq)
{
    run_info_list[0] = 0xFF; run_info_list[1] = 0xEE; run_info_list[2] = 0xDD; run_info_list[3] = 0xCC; // test value
    *error_code = ERR_NO_IMPL;
    (void)seq;
}

// a stub of function 'ReplyToSetLedConfig'
__attribute__((weak)) void ReplyToSetLedConfig(
    serial_datagram_item_t mode,
    serial_datagram_item_t mode_param,
    res_error_code_t *error_code,  serial_datagram_item_t seq)
{
    *error_code = ERR_NO_IMPL;
    (void) mode; (void) mode_param; (void)seq;
}

// a stub of function 'ReplyToSetLaserConfig'
__attribute__((weak)) void ReplyToSetLaserConfig(
    serial_datagram_item_t mode,
    serial_datagram_item_t mode_param,
    res_error_code_t *error_code, serial_datagram_item_t seq)
{
    *error_code = ERR_NO_IMPL;
    (void) mode; (void) mode_param; (void)seq;
}

// a stub of function 'ReplyToSetFlashLightConfig'
__attribute__((weak)) void ReplyToSetFlashLightConfig(
    serial_datagram_item_t mode,
    serial_datagram_item_t mode_param,
    res_error_code_t *error_code, serial_datagram_item_t seq)
{
    *error_code = ERR_NO_IMPL;
    (void) mode; (void) mode_param; (void)seq;
}

// a stub of function 'ReplyToStartFactoryTest'
__attribute__((weak)) void ReplyToStartFactoryTest(
    res_error_code_t *error_code,
    uint32_t *test_item_list,
    serial_datagram_item_t seq)
{
    memset(test_item_list, 0xBB, 10 * sizeof(test_item_list[0]));
    *error_code = ERR_NO_IMPL;
    (void)seq;
}


// generic processing function to specific function for message REQ_HW_FW_VERSION
static res_error_code_t req_hw_fw_version_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)input_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToReqHwFwVersion(&error_code, output_item_list + 0, output_item_list + 1, seq);
    return error_code;
}

// generic processing function to specific function for message REQ_RUN_INFO
static res_error_code_t req_run_info_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)input_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToRunInfo(&error_code, output_item_list, seq);
    return error_code;
}

// generic processing function to specific function for message SET_LED_CONFIG
static res_error_code_t set_led_config_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)output_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToSetLedConfig(input_item_list[0], input_item_list[1], &error_code, seq);
    return error_code;
}

// generic processing function to specific function for message SET_LASER_CONFIG
static res_error_code_t set_laser_config_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)output_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToSetLedConfig(input_item_list[0], input_item_list[1], &error_code, seq);
    return error_code;
}

// generic processing function to specific function for message SET_FLASH_LIGHT_CONFIG
static res_error_code_t set_flash_light_config_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)output_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToSetLedConfig(input_item_list[0], input_item_list[1], &error_code, seq);
    return error_code;
}

// generic processing function to specific function for message START_FACTORY_TEST
static res_error_code_t start_factory_test_msg_proc(
    const serial_datagram_item_t input_item_list[],
    serial_datagram_item_t output_item_list[],
    const serial_datagram_item_t seq)
{
    (void)input_item_list;
    res_error_code_t error_code = ERR_UNKNOWN;
    ReplyToStartFactoryTest(&error_code, output_item_list, seq);
    return error_code;
}

#endif


#if !PROTOCOL_SERVER_SIDE
#include <stdio.h>

// host side, send request to the MCU
// These functions are specific functions, converted into general data transmission
bool ReqHwFwVersion(serial_datagram_item_t seq)
{
    bool ret = serial_datagram_send(seq, REQ_HW_FW_VERSION, NULL, 0);
    return ret;
}

bool ReqRunInfo(serial_datagram_item_t seq)
{
    bool ret = serial_datagram_send(seq, REQ_RUN_INFO, NULL, 0);
    return ret;
}

bool SetLedConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq)
{
    const serial_datagram_item_t param_list[] = { mode, mode_param };
    bool ret = serial_datagram_send(seq, SET_LED_CONFIG, param_list, sizeof param_list / sizeof(param_list[0]));
    return ret;
}

bool SetLaserConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq)
{
    const serial_datagram_item_t param_list[] = { mode, mode_param };
    bool ret = serial_datagram_send(seq, SET_LASER_CONFIG, param_list, sizeof param_list / sizeof(param_list[0]));
    return ret;
}

bool SetFlashLightConfig(serial_datagram_item_t mode, serial_datagram_item_t mode_param, serial_datagram_item_t seq)
{
    const serial_datagram_item_t param_list[] = { mode, mode_param };
    bool ret = serial_datagram_send(seq, SET_FLASH_LIGHT_CONFIG, param_list, sizeof param_list / sizeof(param_list[0]));
    return ret;
}

bool StartFactoryTest(serial_datagram_item_t seq)
{
    bool ret = serial_datagram_send(seq, START_FACTORY_TEST, NULL, 0);
    return ret;
}

// generic processing function to specific function for message REQ_HW_FW_VERSION
static void req_hw_fw_version_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    DispatchReplyOfReqHwFwVersion(error_code, input_item_list[0], input_item_list[1], seq);
}

// generic processing function to specific function for message REQ_RUN_INFO
static void req_run_info_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    DispatchReplyOfRunInfo(error_code, input_item_list, seq);
}

// generic processing function to specific function for message SET_LED_CONFIG
static void set_led_config_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    (void)input_item_list;
    DispatchReplyOfSetLedConfig(error_code, seq);
}

// generic processing function to specific function for message SET_LASER_CONFIG
static void set_laser_config_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    (void)input_item_list;
    DispatchReplyOfSetLaserConfig(error_code, seq);
}

// generic processing function to specific function for message SET_FLASH_LIGHT_CONFIG
static void set_flash_light_config_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    (void)input_item_list;
    DispatchReplyOfSetFlashLightConfig(error_code, seq);
}

// generic processing function to specific function for message START_FACTORY_TEST
static void start_factory_test_msg_proc(
    const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq) 
{
    DispatchReplyOfStartFactoryTest(error_code, input_item_list, seq);
}

// a stub of function 'DispatchReplyOfReqHwFwVersion'
__attribute__((weak)) void DispatchReplyOfReqHwFwVersion(
    const res_error_code_t error_code, const uint32_t HwVersion, const uint32_t FwVersion, serial_datagram_item_t seq)
{
    fprintf(stderr, "received MCU hw/fw version %04X %04X, error_code %u, seq %u\n", HwVersion, FwVersion, error_code, seq);
}

// a stub of function 'DispatchReplyOfRunInfo'
__attribute__((weak)) void DispatchReplyOfRunInfo(
    const res_error_code_t error_code, const uint32_t *run_info_list, serial_datagram_item_t seq)
{
    uint32_t total_datagram          = run_info_list[0];
    uint32_t bad_format_datagram     = run_info_list[1];
    uint32_t total_skipped_uart_recv = run_info_list[2];
    uint32_t total_inner_error       = run_info_list[3];
    fprintf(stderr,
        "received MCU run info total_datagram, bad_format_datagram, total_skipped_uart_recv, total_inner_error,"
        " %X %X %X %X, error_code %u, seq %u\n",
        total_datagram, bad_format_datagram, total_skipped_uart_recv, total_inner_error,
        error_code, seq);
}

// a stub of function 'DispatchReplyOfSetLedConfig'
__attribute__((weak)) void DispatchReplyOfSetLedConfig(const res_error_code_t error_code, serial_datagram_item_t seq)
{
    fprintf(stderr, "received MCU set led config error_code %u, seq %u\n", error_code, seq);
}

// a stub of function 'DispatchReplyOfSetLaserConfig'
__attribute__((weak)) void DispatchReplyOfSetLaserConfig(const res_error_code_t error_code, serial_datagram_item_t seq)
{
    fprintf(stderr, "received MCU set laser config error_code %u, seq %u\n", error_code, seq);
}

// a stub of function 'DispatchReplyOfSetFlashLightConfig'
__attribute__((weak)) void DispatchReplyOfSetFlashLightConfig(const res_error_code_t error_code, serial_datagram_item_t seq)
{
    fprintf(stderr, "received MCU set flash light config error_code %u, seq %u\n", error_code, seq);
}

// a stub of function 'DispatchReplyOfStartFactoryTest'
__attribute__((weak)) void DispatchReplyOfStartFactoryTest(
    const res_error_code_t error_code, const uint32_t *test_item_list, serial_datagram_item_t seq)
{
    fprintf(stderr, "received MCU factory test result error_code %u, seq %u,", error_code, seq);
    for (int i = 0; i < 10; i++) {
        fprintf(stderr, " %X", test_item_list[i]);
    }
    fprintf(stderr, "\n");
}


#endif
