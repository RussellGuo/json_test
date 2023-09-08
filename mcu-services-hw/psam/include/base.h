#ifndef __BASE_H__
#define __BASE_H__

#include <string.h>
#include <stdlib.h>
#include "makefun.h"
#include "mhscpu.h"
#include "mhscpu_gpio.h"
#include "mhscpu_timer.h"

typedef unsigned char byte;
typedef unsigned char u8;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uint8;
typedef unsigned short u16;
typedef unsigned short uint16;
typedef unsigned int u32;
typedef unsigned int uint32;
typedef unsigned long long u64;
typedef unsigned long long uint64;

enum {
    false = 0,
    true,
};

#define MIN(a, b)       ((a) < (b) ? (a) : (b))

#define mdelay  DelayMs
#define udelay  DelayUs

void DelayUs(int us);
void DelayMs(int ms);

#define nGPA    0
#define nGPB    1
#define nGPC    2
#define nGPD    3
#define nGPE    4

unsigned long get_system_frequency(void);
unsigned long get_apb_frequency(void);

#endif
