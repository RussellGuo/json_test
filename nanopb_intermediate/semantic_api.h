
#include "semantic_api.generated.h"
#include "pb_encode.h"
#include "pb_decode.h"

// 处理来自host的报文
bool process_incoming_datagram(const void *data_ptr, pb_size_t len);
