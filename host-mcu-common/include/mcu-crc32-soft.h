#ifndef __MCU_CRC32_SOFT_H__
#define __MCU_CRC32_SOFT_H__

// By Guo Qiang, XBD, Huaqin, 2020-11-25
// a software CRC32 function, which is identical with GD32 hardware CRC32

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


static inline uint32_t mcu_crc32_soft_init(void)
{
    return 0xFFFFFFFFU;
}

static inline uint32_t mcu_crc32_soft_append(uint32_t last_crc, uint32_t data)
{
    uint32_t crc = last_crc;
    uint32_t v= data;
    for (int j = 0; j < 4; j++) {
        crc ^= v & 0xFF000000U;
        v <<= 8;

        for (int k = 0; k < 8; k++) {
            uint32_t msb = crc >> 31;
            crc <<= 1;
            crc ^= (0 - msb) & 0x04C11DB7;
        } // end of for k
   } // end of for j

   return crc;
}

static inline uint32_t mcu_crc32_soft(const uint32_t *message, size_t len)
{
    uint32_t crc = mcu_crc32_soft_init();

    for(size_t i = 0; i < len; i++) {
        uint32_t v = message[i];
        crc = mcu_crc32_soft_append(crc, v);
    }
    return crc;
}

#ifdef __cplusplus
}
#endif

#endif //__MCU_CRC32_SOFT_H__
