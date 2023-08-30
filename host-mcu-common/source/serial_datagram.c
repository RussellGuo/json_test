/*
 * serial_datagram.c

 * Implementation of Datagram Layer of Host-MCU Communication Protocol

 *  Created on: Nov 27, 2020
 *      Author: Guo Qiang
 *
 *  移植到everest的host-mcu上。曹猛在2023-8月初移植的，但这个懒家伙没在这里说明
 *
 *  郭强 2023-8-22日新增了一对"dirty and quick"的调用函数，数据报对语义层接口改成字节对齐的了。未来要单独优化这段。
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "semantic_api.h"
#include "datagram_codec.h"
#include "uart_io_api.h"

#include <ctype.h>

// Read a datagram from the UART.
// Parameters:
//   [out]raw_datagram, the space to store the datagram
//   [in]max_size, the max size of the raw_datagram
//   [out]actual_size_ptr, as its name
//   [out]skipped_byte_count_ptr, unrecognized byte count at this calling
// Return value:
//   true means success, otherwise failure
// Note: Messages with incorrect format will not return an error flag, but will look
// for the next message and record the number of characters skipped. The only possibility
// for error return is that the serial port can no longer be read.
static bool get_raw_datagram_from_serial(
    uint8_t *restrict raw_datagram, size_t max_size,
    size_t *restrict actual_size_ptr, size_t *restrict skipped_byte_count_ptr)
{
    *skipped_byte_count_ptr = 0;
Top:
    // find the SOT
    for (;;) {
        uint8_t byte = 0;
        if (!uart_recv_byte(&byte, (uint32_t)(-1))) {
            // recv no longer OK
            return false;
        }

        if (byte == DATAGRAM_SOT) {
            break;
        }
        (*skipped_byte_count_ptr)++;
    }


    size_t curr_idx;

SoT_found:
    curr_idx = 0;
    raw_datagram[curr_idx++] = DATAGRAM_SOT;
    for(;; curr_idx++) {
        // store every byte recved until a EOT

        if (curr_idx + 1 >= max_size) {
            // no space to store
            (*skipped_byte_count_ptr) += curr_idx;
            //rpc_log(LOG_ERROR, "too long datagram at '%s'", raw_datagram); //log注释先不用
            goto Top;
        }

        if (!uart_recv_byte(raw_datagram + curr_idx, (uint32_t)(-1))) {
            // recv no longer OK
            return false;
        }

        if (raw_datagram[curr_idx] == DATAGRAM_EOT) {
            break;
        }

        if (raw_datagram[curr_idx] == DATAGRAM_SOT) {
            (*skipped_byte_count_ptr) += curr_idx;
            raw_datagram[curr_idx] = 0;
            //rpc_log(LOG_ERROR, "met a SoT in lookup EoT at '%s'", raw_datagram); //log注释先不用
            goto SoT_found;
        }
    }

    *actual_size_ptr = curr_idx + 1;
    return true;
}


// This function will read the UART, get the datagram string one after another,
// and convert the string into the datagram binary format
// (that is, a 32-bit unsigned integer array, and separate the sequence number,
// message ID, parameter/return Value sequence and CRC), check CRC.
// Finally, the message is dispatched by calling the interface of the semantic layer.
// Note: Messages with incorrect format will not return an error flag, but will look
// for the next message and record the number of characters skipped. The only possibility
// for error return is that the serial port can no longer be read. At this time, the
// entire receiving task will be aborted and returned
void serial_datagram_receive_loop(void *arg)
{
    (void)arg;
    for(;;) { // for each datagram
        // get raw-data
        char datagram_str[MAX_DATAGRAM_LEN];
        size_t datagram_str_size;
        uint8_t decoded_data[MAX_DATAGRAM_LEN] = {0};
        size_t decoded_data_len = sizeof decoded_data;
        size_t skipped_count;

        memset(datagram_str, 0, sizeof datagram_str);
        memset(decoded_data, 0, sizeof decoded_data);
        int ret = get_raw_datagram_from_serial( (uint8_t *)datagram_str, sizeof datagram_str, &datagram_str_size, &skipped_count);
        if (!ret) {
            return;
        }
        ret = decode_from_datagram(decoded_data,&decoded_data_len,datagram_str,datagram_str_size); // <数据报> -> 数据报data
        if(ret)
        {
            ret = process_incoming_datagram(decoded_data, decoded_data_len);
        }else{
            return;
        }
    } // for each datagram
}

__attribute__((weak)) bool process_incoming_datagram(const void *data_ptr, unsigned short len) {
    (void)data_ptr;
    (void)len;
    return false;
}

bool send_datagram(const void *data_ptr, unsigned short len) {
    bool ret;
    if (len == 0) {
        return false;
    }
    uint8_t data_gram_buf[MAX_DATAGRAM_LEN] = {0};
    size_t datagram_len = sizeof data_gram_buf;
    ret = encode_to_datagram(data_gram_buf, &datagram_len, data_ptr, len);
    if (ret) {
        ret = uart_send_data(data_gram_buf, datagram_len, (uint32_t)(-1));
    }
    return ret;
}
