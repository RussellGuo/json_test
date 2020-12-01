/*
 * serial_datagram_common.h

 * Implementation of Datagram Layer of Host-MCU Communication Protocol

 *  Created on: Nov 27, 2020
 *      Author: Guo Qiang
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "serial_datagram.h"

#include "mcu-crc32-soft.h"
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
static bool get_raw_datagram_from_serial(
    uint8_t *restrict raw_datagram, size_t max_size,
    size_t *restrict actual_size_ptr, size_t *restrict skipped_byte_count_ptr)
{
    *skipped_byte_count_ptr = 0;

    // found the SOT
    for (;;) {
        uint8_t byte = 0;
        if (!uart_recv_byte(&byte, (uint32_t)(-1))) {
            // TODO: Log error record
            return false;
        }

        if (byte == SERIAL_DATAGRAM_START_CHR) {
            break;
        }
        (*skipped_byte_count_ptr)++;
    }

    size_t curr_idx;
    for(curr_idx = 0;; curr_idx++) {
        // store every byte recved until a EOT

        if (curr_idx + 1 >= max_size) {
            // TODO: Log error record
            return false;
        }

        if (!uart_recv_byte(raw_datagram + curr_idx, (uint32_t)(-1))) {
            // TODO: Log error record
            return false;
        }

        if (raw_datagram[curr_idx] == SERIAL_DATAGRAM_STOP_CHR) {
            raw_datagram[curr_idx] = 0;
            break;
        }
    }
    *actual_size_ptr = curr_idx;
    // TODO: global_run_info.total_recv_datagram++;
    return true;
}

// This function will read the UART, get the datagram string one after another,
// and convert the string into the datagram binary format
// (that is, a 32-bit unsigned integer array, and separate the sequence number,
//message ID, parameter/return Value sequence and CRC), check CRC.
// Finally, the message is dispatched by calling the interface of the semantic layer.
void serial_datagram_receive_loop(void *arg)
{
    (void)arg;
    for(;;) { // for echo datagram
        // get raw-data
        char datagram_str[MAX_DATAGRAM_STR_LEN + 1];
        size_t datagram_str_size;
        size_t skipped_count;

        memset(datagram_str, 0, sizeof datagram_str);
        int ret = get_raw_datagram_from_serial( (uint8_t *)datagram_str, sizeof datagram_str, &datagram_str_size, &skipped_count);
        if (!ret) {
            // TODO: record log
            continue;
        }

        if (datagram_str[0] == 'L') {
            // LOG datagram
            // TODO: Log message process
        } else {
            // command/response datagram
            // to store items of datagram
            serial_datagram_item_t items[MAX_DATAGRAM_ITEM_COUNT];
            size_t item_count = 0;

            bool isOK = true;

            // check char set
            for (size_t i = 0; i < datagram_str_size; i++) {
                if (isxdigit(datagram_str[i]) || datagram_str[i] == ' ') {
                    continue;
                } else {
                    // TODO: Log error record
                    isOK = false;
                    break;
                }
            }

            // separate values
            if (isOK) {
                for(char *p = datagram_str; p != NULL; p = strchr(p, ' ')) {
                    if (item_count >= sizeof(items) / sizeof(items[0])) {
                        // TODO: Log error record
                        isOK = false;
                        break;
                    }

                    if (p[0] == ' ') {
                        p++;
                    }
                    isOK = sscanf(p, "%X", items + item_count) == 1;
                    item_count++;

                    if (!isOK) {
                        // TODO: Log error record
                        // Don't think it will be happened
                        break;
                    }
                }
            }

            // check CRC and item count
            if (isOK) {
                // seq msg_id crc, at least 3 items.
                // crc checked
                isOK = item_count >= 3 && mcu_crc32_soft(items, item_count) == 0;
            }
            if (isOK) {
                // dispatch the message
                serial_datagram_arrived(items[0], items[1], items + 2, item_count - 3);
            } else {
                // to record mismatch datagram.
                //report_mismatch_raw_datagram(buf, buf_size);
            }
        } // end of command/response datagram

        // skipped byte recording
        if (skipped_count) {
            //report_skipped_bytes_before_get_raw_datagram(skipped_count);
        }
    } // // end of echo datagram

}

// This function is used to send a datagram.
// Parameters:
//   [in]seq, msg_id, data list and data count
// Return value:
//   true means success, otherwise failure

bool serial_datagram_send(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len)
{
    char buf[MAX_DATAGRAM_STR_LEN + 1];

    uint32_t crc = mcu_crc32_soft_init();
    crc = mcu_crc32_soft_append(crc, seq);
    crc = mcu_crc32_soft_append(crc, msg_id);

    size_t out_len = sprintf(buf, "%c%X %X", SERIAL_DATAGRAM_START_CHR, seq, msg_id);

    for (size_t i = 0; i < len; i++) {
        // one item of a datagram's max size is 9(hexdecimal and a space),
        // right here the buf needs 1 item, 1 crc, 1 <EOT>, 1 '\0'
        if (out_len + 9 + 9 + 2 >= sizeof buf) {
            // TODO: Log error record
            return false;
        }

        serial_datagram_item_t value = data_list[i];
        out_len += sprintf(buf + out_len, " %X", value);
        crc = mcu_crc32_soft_append(crc, value);
    }

    out_len += sprintf(buf + out_len, " %X%c", crc, SERIAL_DATAGRAM_STOP_CHR);
    bool ret = uart_send_data((const uint8_t *)buf, out_len, (uint32_t)(-1));
    return ret;
}

