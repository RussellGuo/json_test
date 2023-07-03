#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "mhscpu.h"
#include "string.h"

#ifdef __cplusplus
extern "c" {
#endif

#define RING_BUFFER_SIZE		1024

typedef struct RingBuffer
{
	uint8_t buffer[RING_BUFFER_SIZE];
	uint32_t put_index, get_index;
	uint32_t count;
}RingBufferTypeDef;

void ringBufferInit(RingBufferTypeDef * buf);
void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch);
int32_t ringBufferGet(RingBufferTypeDef * buf);

#ifdef __cplusplus
}
#endif

#endif
