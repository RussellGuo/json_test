#ifndef __CRC_API_H__
#define __CRC_API_H__

// By Guo Qiang, XBD, Huaqin, 2020-11-26
// a warp of CRC.
// there are 2 implements:
//    the SW one, processor independ version, placed in host-mcu-common/source/crc32-API.c
//    the HW one, GD32 MCU only, placed in common/source/crc32-API.c

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// init the CRC module
// parameters: NONE
// return value: true if successed, vice versa
bool crc_init(void);

// calc the CRC32 for an array
// [in] data, the array
// [in] len, data count
// [out] crc32, the ptr of the result
// return value: true if successed, vice versa
bool crc32_array(const uint32_t *data, const size_t len, uint32_t *crc32);

#endif // __CRC_API_H__

