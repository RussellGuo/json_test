/*
 * mcu-common.h
 *
 * Common function at the MCU side.
 *
 *  Created on: Dec 9, 2020
 *      Author: Guo Qiang
 */

#ifndef __MCU_COMMON_H__
#define __MCU_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "cmsis_os2.h"
#include "stdbool.h"

// sleep ticks or thread flag be set
// parameters:
//   [in] flag, waiting thread flag
//   [in] ticks, waiting time in os tick
// return value:
//   return true if thread flag be set, otherwise (mostly) time out or error
inline static bool wait_flag_sleep(uint32_t flag, uint32_t ticks)
{
    int32_t wait = osThreadFlagsWait(flag, osFlagsWaitAny, ticks);
    return wait == flag;
}

// always sleep until thread flag be set or error
// parameters:
//   [in] flag, waiting thread flag
// return value:
//   return true if thread flag be set, otherwise failure
inline static bool wait_flag(uint32_t flag)
{
    bool ret = wait_flag_sleep(flag, osWaitForever);
    return ret;
}

#ifdef __cplusplus
}
#endif


#endif // __MCU_COMMON_H__
