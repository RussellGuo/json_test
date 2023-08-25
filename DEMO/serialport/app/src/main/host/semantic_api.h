//
// Created by ubuntu on 2023/8/25.
//

#ifndef SERIALPORT_SEMANTIC_API_H
#define SERIALPORT_SEMANTIC_API_H
#include "semantic_api.generated.h"
#include "pb_encode.h"
#include "pb_decode.h"

// 处理来自host的报文
bool process_incoming_datagram(const void *data_ptr, unsigned short len);
#endif //SERIALPORT_SEMANTIC_API_H
