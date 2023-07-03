#include "ring_buffer.h"

void ringBufferInit(RingBufferTypeDef * buf)
{
	memset(buf->buffer,0,sizeof(buf->buffer));
	buf->get_index = 0;
	buf->put_index = 0;
	buf->count = 0;
}

void ringBufferPut(RingBufferTypeDef * buf, uint16_t ch)
{
	buf->buffer[buf->put_index] = ch;
	buf->put_index = (buf->put_index + 1) & (RING_BUFFER_SIZE - 1);
	buf->count += 1;
	if(buf->put_index == buf->get_index)
	{
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
}

int32_t ringBufferGet(RingBufferTypeDef * buf)
{
	int32_t ch;
	ch = -1;
	if(buf->get_index != buf->put_index)
	{
		ch = buf->buffer[buf->get_index];
		buf->count -= 1;
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
	return ch;
}
