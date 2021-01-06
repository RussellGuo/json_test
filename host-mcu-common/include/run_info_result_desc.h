/*
 * run_info_result_desc.h
 *
 * definition of run info. run info is the counting data of some information in the data link
 * layer (UART) and datagram layer during the operation of the protocol (including the host side and the MCU side)
 * These count data are stored in an internal array. This header file defines how to access the count data.
 *  Created on: Dec 23, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */


#ifndef __RUN_INFO_RESULT_DESC_H_
#define __RUN_INFO_RESULT_DESC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "serial_datagram.h"

// The index of the counting array. Currently 6 are defined and 4 are reserved
typedef enum {
    RUN_INFO_IDX_UART_RECV_COUNT,
    RUN_INFO_IDX_UART_RECV_DROPPED_COUNT,
    RUN_INFO_IDX_UART_RECV_ERROR_COUNT,
    RUN_INFO_IDX_UART_INNER_ERROR_COUNT,
    RUN_INFO_IDX_DATAGRAM_COUNT,
    RUN_INFO_IDX_MISMATCHED_DATAGRAM_COUNT,
    RUN_INFO_IDX_MAX = 9,
} run_info_idx_t;


// return the writable address of the counting array. Private function.
// don't use this function out of this .h file. It's for inner using only.
static inline serial_datagram_item_t *__inner_get_local_run_info_tab(void)
{
    extern serial_datagram_item_t inner_run_info_tab[];
    return inner_run_info_tab;
}


// return the read-only address of the counting array. public function
// It can be used likes: get_local_run_info_tab()[RUN_INFO_IDX_***_COUNT]
// it mostly be used in host side.
// MCU side will return the values to the host by the protocol with msg_id 'REQ_RUN_INFO'
static inline const serial_datagram_item_t *get_local_run_info_tab(void)
{
    return __inner_get_local_run_info_tab();
}

// Use macros to implement C template functions.
// The template parameters are the function name and the index of the count array (both in short form).
// The template function itself accepts a cumulative parameter n.
// n will be accumulated to  the corresponding count data index.
// create a public function record_NAME, it would accumulate count data with index 'idx' 'n'
#define RECORD_RUN_INFO(name, idx) \
static inline void record_##name(size_t n) {\
    serial_datagram_item_t *tab = __inner_get_local_run_info_tab(); \
    tab[RUN_INFO_IDX_##idx##_COUNT] += n; \
}

// create 6 public funtions.
RECORD_RUN_INFO(uart_recv          , UART_RECV          )
RECORD_RUN_INFO(uart_recv_dropped  , UART_RECV_DROPPED  )
RECORD_RUN_INFO(uart_recv_error    , UART_RECV_ERROR    )
RECORD_RUN_INFO(uart_inner_error   , UART_INNER_ERROR   )
RECORD_RUN_INFO(datagram           , DATAGRAM           )
RECORD_RUN_INFO(mismatched_datagram, MISMATCHED_DATAGRAM)

#ifdef __cplusplus
}
#endif

#endif //__RUN_INFO_RESULT_DESC_H_
