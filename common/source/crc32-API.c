#include "crc32-API.h"

// By Guo Qiang, XBD, Huaqin, 2020-11-26
// a warp of CRC.
//    the HW implement, GD32 MCU only.


#include "gd32e10x_crc.h"

#include "cmsis_os2.h"

// a mutex to avoid crosstalk between multiple threads
static osMutexId_t mutex_crc_hw_id;

// init the CRC module
// parameters: NONE
// return value: true if successed, vice versa
bool crc_init(void)
{
    bool ret = mutex_crc_hw_id = osMutexNew (0);
    if (ret) {
        rcu_periph_clock_enable(RCU_CRC);
        crc_deinit();
    }
    return ret;
}

// calc the CRC32 for an array
// [in] data, the array
// [in] len, data count
// [out] crc32, the ptr of the result
// return value: true if successed, vice versa
bool crc32_array(const uint32_t *data, const size_t len, uint32_t *crc32)
{
    bool ret = osMutexAcquire(mutex_crc_hw_id, osWaitForever) == osOK;
    if (ret) {
        crc_data_register_reset();
        *crc32 = crc_block_data_calculate(data, len);
        osMutexRelease(mutex_crc_hw_id);
    }
    return ret;
}
