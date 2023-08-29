#ifndef __CRC8_SOFT_H__
#define __CRC8_SOFT_H__

/*
    CRC8计算，来自往上Lammert Bies的程序，MIT license（可以随便用但作者不负责）
    https://github.com/lammertb/libcrc/blob/master/src/crc8.c

    郭强 2023-8-24
*/

#include <stddef.h>
#include <stdint.h>

// 一目了然
uint8_t crc8_soft(const void *ptr, size_t len);

#endif
