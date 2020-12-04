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

#if PROTOCOL_SERVER_SIDE
#define MSG_PROC_PROTOTYPE const serial_datagram_item_t input_item_list[], serial_datagram_item_t output_item_list[], const serial_datagram_item_t seq
#define MSG_PROC_RET_TYPE res_error_code_t
#else
#define MSG_PROC_PROTOTYPE const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq
#define MSG_PROC_RET_TYPE void
#endif

MSG_PROC_RET_TYPE              req_hw_fw_msg_proc(MSG_PROC_PROTOTYPE);
MSG_PROC_RET_TYPE           req_run_info_msg_proc(MSG_PROC_PROTOTYPE);
MSG_PROC_RET_TYPE         set_led_config_msg_proc(MSG_PROC_PROTOTYPE);
MSG_PROC_RET_TYPE       set_laser_config_msg_proc(MSG_PROC_PROTOTYPE);
MSG_PROC_RET_TYPE set_flash_light_config_msg_proc(MSG_PROC_PROTOTYPE);
MSG_PROC_RET_TYPE     start_factory_test_msg_proc(MSG_PROC_PROTOTYPE);


typedef MSG_PROC_RET_TYPE (*semantic_msg_process_t)(MSG_PROC_PROTOTYPE);

typedef struct {
    msg_id_t msg_id;
    bool direction_to_mcu;
    uint8_t req_len;
    uint8_t res_len;
    semantic_msg_process_t msg_proc;
} sematic_layer_info_t;

static const sematic_layer_info_t *get_sematic_layer_info(serial_datagram_item_t msg_id);

static const sematic_layer_info_t sematic_layer_info_tab[] = {
    { REQ_HW_FW_VERSION     , true , 0,  2, req_hw_fw_msg_proc              },
    { REQ_RUN_INFO          , true , 0,  4, req_run_info_msg_proc           },
    { SET_LED_CONFIG        , true , 2,  1, set_led_config_msg_proc         },
    { SET_LASER_CONFIG      , true , 2,  1, set_laser_config_msg_proc       },
    { SET_FLASH_LIGHT_CONFIG, true , 2,  1, set_flash_light_config_msg_proc },
    { START_FACTORY_TEST    , true,  0, 10, start_factory_test_msg_proc     },
};

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

// Once a datagram be received from remote, this function will be invoked
// Parameters:
//   [in]seq, msg_id, data list and data count
void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len)
{
    res_error_code_t error_code = NO_ERROR;
    const sematic_layer_info_t *const sematic_layer_info = get_sematic_layer_info(msg_id);
    if (sematic_layer_info == 0 || !sematic_layer_info->direction_to_mcu) {
        error_code = ERR_MSG_ID;
    }
    if (error_code == NO_ERROR) {
        if (sematic_layer_info->req_len != len) {
            error_code = ERR_ITEM_COUNT;
        }
    }

    serial_datagram_item_t res_items[MAX_DATAGRAM_ITEM_COUNT] = {0};
    if (error_code == NO_ERROR) {
        error_code = sematic_layer_info->msg_proc(data_list, res_items + 1, seq);
    }

    // TODO: now, reply it
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
    const sematic_layer_info_t *const sematic_layer_info = get_sematic_layer_info(msg_id);
    if (sematic_layer_info == 0) {
        // log an ERR_MSG_ID
        return;
    }
    if (sematic_layer_info->res_len + 1 != len) {
        // log an item count error
        return;
    }

    sematic_layer_info->msg_proc(data_list + 1, (res_error_code_t)data_list[0], seq);
}

#endif

#if PROTOCOL_SERVER_SIDE

static void weak_msg_proc(const char *name, const serial_datagram_item_t input_item_list[], serial_datagram_item_t output_item_list[], const serial_datagram_item_t seq)
{
    (void) name;
    (void) input_item_list; (void)output_item_list; (void)seq;
}

#define DEFINE_A_WEAK_MSG_PROC(name) __attribute__((weak)) MSG_PROC_RET_TYPE name(MSG_PROC_PROTOTYPE) \
{ \
    weak_msg_proc(#name, input_item_list,output_item_list,seq);\
    return ERR_EXEC; \
}
#else
#include <stdio.h>
static void weak_msg_proc(const char *name, const serial_datagram_item_t input_item_list[], res_error_code_t error_code, const serial_datagram_item_t seq)
{
    fprintf(stderr, "got a reponse of '%s' #%X, return %x:", name, seq, error_code);
    for(size_t i = 0; i < MAX_DATAGRAM_ITEM_COUNT; i++) {
        fprintf(stderr, " %X", input_item_list[i]);
    }
    fprintf(stderr, "\n");
}

#define DEFINE_A_WEAK_MSG_PROC(name) __attribute__((weak)) MSG_PROC_RET_TYPE name(MSG_PROC_PROTOTYPE) \
{ \
    weak_msg_proc(#name, input_item_list, error_code, seq); \
}
#endif

DEFINE_A_WEAK_MSG_PROC(req_hw_fw_msg_proc)
DEFINE_A_WEAK_MSG_PROC(req_run_info_msg_proc)
DEFINE_A_WEAK_MSG_PROC(set_led_config_msg_proc)
DEFINE_A_WEAK_MSG_PROC(set_laser_config_msg_proc)
DEFINE_A_WEAK_MSG_PROC(set_flash_light_config_msg_proc)
DEFINE_A_WEAK_MSG_PROC(start_factory_test_msg_proc)
