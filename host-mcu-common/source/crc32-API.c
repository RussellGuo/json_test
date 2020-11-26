#include "crc32-API.h"

// By Guo Qiang, XBD, Huaqin, 2020-11-26
// a warp of CRC.
// the SW implement, processor independ version

#include "mcu-crc32-soft.h"


// init the CRC module
// parameters: NONE
// return value: true if successed, vice versa
bool crc_init(void)
{
    return true;
}

// calc the CRC32 for an array
// [in] data, the array
// [in] len, data count
// [out] crc32, the ptr of the result
// return value: true if successed, vice versa
bool crc32_array(const uint32_t *data, const size_t len, uint32_t *crc32)
{
    *crc32 = mcu_crc32_soft(data, len);
    return true;
}
