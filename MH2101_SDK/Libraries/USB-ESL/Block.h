#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "USBESL.h"

typedef struct BlockStruct_s BlockStruct;

struct BlockStruct_s {
    uint32_t* Buffer;
    uint32_t  Size;
    void (*PopCallback)(BlockStruct* block);
    void* Handle;

    volatile bool Locked;
};

#endif
