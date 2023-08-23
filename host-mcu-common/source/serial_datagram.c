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

#include "serial_datagram.h"

#include "mcu-crc32-soft.h"
#include "uart_io_api.h"

//#include "run_info_result_desc.h" //注释先不用

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

        if (byte == SERIAL_DATAGRAM_START_CHR) {
            break;
        }
        (*skipped_byte_count_ptr)++;
    }


    size_t curr_idx;

SoT_found:

    for(curr_idx = 0;; curr_idx++) {
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

        if (raw_datagram[curr_idx] == SERIAL_DATAGRAM_STOP_CHR) {
            raw_datagram[curr_idx] = 0;
            break;
        }

        if (raw_datagram[curr_idx] == SERIAL_DATAGRAM_START_CHR) {
            (*skipped_byte_count_ptr) += curr_idx;
            raw_datagram[curr_idx] = 0;
            //rpc_log(LOG_ERROR, "met a SoT in lookup EoT at '%s'", raw_datagram); //log注释先不用
            goto SoT_found;
        }
    }

    *actual_size_ptr = curr_idx;
    //record_datagram(1);//注释先不用
    return true;
}

//log注释先不用
//#if !defined(IS_MCU_SIDE)
//static void dispatch_mcu_log(const char *msg_str);
//#  endif

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
        char datagram_str[MAX_DATAGRAM_STR_LEN + 1];
        size_t datagram_str_size;
        size_t skipped_count;

        memset(datagram_str, 0, sizeof datagram_str);
        int ret = get_raw_datagram_from_serial( (uint8_t *)datagram_str, sizeof datagram_str, &datagram_str_size, &skipped_count);
        if (!ret) {
            // cannot recovery the recv because of UART
            //rpc_log(LOG_FATAL, "recv from UART no longer OK"); //log注释先不用
            return;
        }
        //rpc_log(LOG_VERBOSE, "got a datagram '%s'", datagram_str);
        if (skipped_count) {
            //rpc_log(LOG_WARN, "skipped %d char(s) before datagram '%s'", skipped_count, datagram_str); //log注释先不用
            //record_uart_recv_dropped(skipped_count);//注释先不用
        }

        if (datagram_str[0] == 'L') {
            //注释先不用
            // LOG datagram
            //#if !defined(IS_MCU_SIDE)
            //dispatch_mcu_log(datagram_str + 1);
            //#else
            //rpc_log(LOG_ERROR, "host should not send log: '%s'", datagram_str);
            // to record mismatch datagram.
            //record_mismatched_datagram(1);
            //#endif
            continue;
        }
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
                //rpc_log(LOG_ERROR, "bad char at '%s'", datagram_str); //log注释先不用
                isOK = false;
                break;
            }
        }

        // separate values
        if (isOK) {
            for(char *p = datagram_str; p != NULL; p = strchr(p, ' ')) {
                if (item_count >= sizeof(items) / sizeof(items[0])) {
                    //rpc_log(LOG_ERROR, "too many chars at '%s'", datagram_str); //log注释先不用
                    isOK = false;
                    break;
                }

                if (p[0] == ' ') {
                    p++;
                }
                isOK = sscanf(p, "%X", items + item_count) == 1;
                item_count++;

                if (!isOK) {
                    // Don't think it will be happened
                    //rpc_log(LOG_ERROR, "non-hexdecimal char at '%s'", datagram_str); //log注释先不用
                    break;
                }
            }
        }

        // check CRC and item count
        if (isOK) {
            // seq msg_id crc, at least 3 items.
            // crc checked
            isOK = item_count >= 3 && mcu_crc32_soft(items, item_count) == 0;
            if (!isOK) {
                //rpc_log(LOG_ERROR, "item too less or CRC mismatch at '%s'", datagram_str); //log注释先不用
            }
        }
        if (isOK) {
            // dispatch the message
            printf("datagram_str:%x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", datagram_str[0], datagram_str[1], datagram_str[2], datagram_str[3], datagram_str[4], datagram_str[5],
            datagram_str[6], datagram_str[7], datagram_str[8], datagram_str[9], datagram_str[10], datagram_str[11], datagram_str[12], datagram_str[13]);
            serial_datagram_arrived(items[0], items[1], items + 2, item_count - 3); //注释先不用
        } else {
            //record_mismatched_datagram(1); //注释先不用
        }
    } // for each datagram

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
            // no space
            //rpc_log(LOG_ERROR, "too long sending datagram at seq %u", seq); //log注释先不用
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

//log注释先不用
/*
// The lowest log output level. Logs of lower levels will not be processed
static log_level_t min_log_level = LOG_INFO;

// set the lowest log output level. Logs of lower levels will not be processed
// parameters: [in] log_level as its name
void set_rpc_log_level(log_level_t log_level)
{
    // clamp log level
    if (log_level > LOG_FATAL) {
        min_log_level = LOG_FATAL;
    } else if (log_level < LOG_VERBOSE) {
        min_log_level = LOG_VERBOSE;
    } else {
        min_log_level = log_level;
    }
}

// return the lowest log output level.
log_level_t get_rpc_log_level(void)
{
    return min_log_level;
}


#if defined(IS_MCU_SIDE)

// send the log via protocol at the MCU side
// a printf like function, log_level as it name
// parameters: [in] for all
// return value:true means success, otherwise failure
bool rpc_log(log_level_t log_level, const char *format, ...)
{
    // ignore low-level log
    if (log_level < min_log_level) {
        return true;
    }

    char buf[MAX_DATAGRAM_STR_LEN + 1];
    memset(buf, 0, sizeof buf);

    // clamp log level
    log_level_t _log_level;
    if (log_level > LOG_FATAL) {
        _log_level = LOG_FATAL;
    } else if (log_level < LOG_VERBOSE) {
        _log_level = LOG_VERBOSE;
    } else {
        _log_level = log_level;
    }

    // encoding the protocol datagram
    // start char
    size_t out_len = sprintf(buf, "%cL%c", SERIAL_DATAGRAM_START_CHR, '0' + _log_level);

    // msg
    va_list args;
    va_start(args, format);
    int printed_max = (sizeof buf) - 4;  // 4 chars: <SOT> L 0 <EOT>
    int printed = vsnprintf(buf + out_len, printed_max, format, args); // pls hanle this very careful
    va_end(args);
    if (printed < 0) {
        return false;
    }
    if (printed > printed_max) {
        // vsnprintf() do not write more than printed_max bytes, a bigger value means
        // that the output was truncated. recalculate the actual size of printed.
        printed = printed_max;
    }
    out_len += printed;

    // stop char
    out_len += sprintf(buf + out_len, "%c", SERIAL_DATAGRAM_STOP_CHR);


    // send datagram
    bool ret = uart_send_data((const uint8_t *)buf, out_len, (uint32_t)(-1));
    return ret;
}

#else

// host size, process the protocol log
// a printf like function, log_level as it name
// parameters: [in] for all
// return value:true means success, otherwise failure
bool rpc_log(log_level_t log_level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    // host side log has a 'HST' tag. invoke the customizable core log func
    rpc_logv(log_level, "HST", format, args);
    va_end(args);
    return true;
}

// helper func to dispatche_mcu_log. only for it
static void _rpc_logv(log_level_t log_level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    // MCU side log has a 'MCU' tag. invoke the customizable core log func
    rpc_logv(log_level, "MCU", format, args);
    va_end(args);
}

// process a log datagram at the host side
// parameter: msg_str, undecoded log string.
static void dispatch_mcu_log(const char *msg_str)
{
    // decode the log_level, clamp it
    const uint8_t log_chr = msg_str[0];
    log_level_t _log_level;
    if (log_chr > '0' + LOG_FATAL) {
        _log_level = LOG_FATAL;
    } else if (log_chr < '0' + LOG_VERBOSE) {
        _log_level = LOG_VERBOSE;
    } else {
        _log_level = log_chr - '0';
    }

    // call the core log func
    _rpc_logv(_log_level, "%s", msg_str + 1);
}

// customizable core log func at the host side
// parameters:
// [in] log_level as its name
// [in] tag, could be "HST" or "MCU"
// [in] format and args, vprintf like
__attribute__((weak)) void rpc_logv(log_level_t log_level, const char *tag, const char *format, va_list args)
{
    // clamp log level
    log_level_t _log_level;
    if (log_level > LOG_FATAL) {
        _log_level = LOG_FATAL;
    } else if (log_level < LOG_VERBOSE) {
        _log_level = LOG_VERBOSE;
    } else {
        _log_level = log_level;
    }

    // ignore low-level log
    if (_log_level < get_rpc_log_level()) {
        return;
    }
    // found the log name
    static const char *log_level_name_tab[] = {
        "verbose", "debug", "info", "warn", "error", "fatal",
    };
    const char *const log_name = log_level_name_tab[_log_level - LOG_VERBOSE];

    // output to the stderr
    fprintf(stderr, "LOG '%s' %s ", log_name, tag);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

#endif
*/

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
    size_t item_size = ((len - 1) / 4 + 1);
    ret = serial_datagram_send(0, 0, (const serial_datagram_item_t *)data_ptr, item_size);
    return ret;
}

void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id, const serial_datagram_item_t *restrict data_list, const size_t len) {
    (void)seq;
    (void)msg_id;
    process_incoming_datagram(data_list, len * 4);
}
