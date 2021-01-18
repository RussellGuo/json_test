/*
 * task_serial_datagram_recv.h
 *
 * task of protocol datagram receiving
 * this task is the main task of the MCU side, and should be start at main()
 *
 *  Created on: Nov 25, 2020
 *      Author: Guo Qiang
 */

#ifndef __MCU_HW_COMMON_H__
#define __MCU_HW_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


// start task of protocol datagram receiving
// parameter: none
// return value: true if done; otherwise failed
bool init_thread_of_serial_datagram_recv(void);

#ifdef __cplusplus
}
#endif

#endif // __MCU_HW_COMMON_H__
