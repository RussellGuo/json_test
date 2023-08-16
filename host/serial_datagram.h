/*
 * serial_datagram.h
 *
 * Implementation of Datagram Layer of Host-MCU Communication Protocol
 *
 *  Created on: Nov 27, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */

#ifndef __SERIAL_DATAGRAM_H_
#define __SERIAL_DATAGRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "misc.h"

// datagram delimiter description
#define SERIAL_DATAGRAM_START_CHR '\r'
#define SERIAL_DATAGRAM_STOP_CHR  '\n'

#define MAX_DATAGRAM_ITEM_COUNT 15
// legth of a hexdecimal with a delimiter is 9(except the 1st, it's 9), plus SOT/EOT
#define MAX_DATAGRAM_STR_LEN (MAX_DATAGRAM_ITEM_COUNT * 9 + 1)

// every data item inside the semantic layer are uint32_t
typedef uint32_t serial_datagram_item_t;

// those are main API entries of receiving and sending, very important

// This function will read the UART, get the datagram string one after another,
// and convert the string into the datagram binary format
// (that is, a 32-bit unsigned integer array, and separate the sequence number,
//message ID, parameter/return-value sequence and CRC), check CRC.
// Finally, the message is dispatched by calling the interface of the semantic layer.
// Note: Messages with incorrect format will not return an error flag, but will look
// for the next message and record the number of characters skipped. The only possibility
// for error return is that the serial port can no longer be read. At this time, the
// entire receiving task will be aborted and returned
void serial_datagram_receive_loop(void *arg);

//语义层先不用
// Once a datagram be received from remote, this function will be invoked
// Parameters:
//   [in]seq, msg_id, data list and data count
//void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
//    const serial_datagram_item_t *restrict data_list, const size_t len);

// This function is used to send a datagram.
// Parameters:
//   [in]seq, msg_id, data list and data count
// Return value:
//   true means success, otherwise failure
bool serial_datagram_send(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len);


// should be implemented by monitor progress, for diagnosis
void report_mismatch_raw_datagram(const void *raw_datagram, size_t raw_datagram_len);
void report_skipped_bytes_before_get_raw_datagram(size_t count);

// Both host and MCU have log support. The function prototype of log is highly similar to Android.
// Because the communication protocol codes of host and MCU are the same, so the invoking of log
//  is also the same, both are functions named rpc_log. The implementation of them is not the same.
// The MCU log is all redirected to the host, and the host outputs the log sent from the MCU
//   to its log system. This log is derived from MCU statements and sent to the host through
//   the protocol. After the host protocol recognizes the log, it will be processed.
// The processing function is rpc_logv
// The rpc_log on the host side is processed directly and not transmitted. The processing function
//  is still rpc_logv.
// The usage of rpc_logv is very similar to __android_log_vprint of Android NDK.
// A 'WEAK' version of rpc_logv has been provided, which directly outputs to stderr.
// Developers can define their own rpc_logv. For example, under the Android NDK, call __android_log_vprint directly.
//#if !defined(IS_MCU_SIDE)
//void rpc_logv(log_level_t log_level, const char *tag, const char *format, va_list args);
//#endif

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_DATAGRAM_H_ */
