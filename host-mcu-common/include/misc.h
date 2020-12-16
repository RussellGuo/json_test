/*
 * misc.h
 *
 * common functions for both the MCU and the host
 *
 *  Created on: Dec 9, 2020
 *      Author: Guo Qiang
 */

#ifndef __MISC_H_
#define __MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// log system

// log level is similar to the Android's
typedef enum {
    LOG_VERBOSE = 2, // keep the number be same as Android's
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
} log_level_t;

// The MCU log should use rpc_log.
// The host side is only used in the implementation of the protocol,
//   and other applications and services directly use the system log.

// a printf like function, log_level as it name
// parameters: [in] for all
// return value:true means success, otherwise failure
bool rpc_log(log_level_t log_level, const char *format, ...);

// set the lowest log output level. Logs of lower levels will not be processed
// parameters: [in] log_level as its name
void set_rpc_log_level(log_level_t log_level);

// return the lowest log output level.
log_level_t get_rpc_log_level(void);

#ifdef __cplusplus
}
#endif

#endif // __MISC_H_
