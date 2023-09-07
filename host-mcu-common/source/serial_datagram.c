/*
 * serial_datagram.c

 * Implementation of Datagram Layer of Host-MCU Communication Protocol

 *  Created on: Nov 27, 2020
 *      Author: Guo Qiang
 *
 *  移植到everest的host-mcu上。曹猛在2023-8月初移植的，但这个懒家伙没在这里说明
 *
 *  郭强 2023-8-22日新增了一对"dirty and quick"的调用函数，数据报对语义层接口改成字节对齐的了。未来要单独优化这段。
 *
 *  曹猛 2023-8-30日替换数据报层的数据处理, 增加数据报文编解码
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "semantic_api.h"
#include "datagram_codec.h"
#include "uart_io_api.h"

#include <ctype.h>

/*从UART读取数据报，
  raw_datagram 存储数据报的空间 out
  max_size raw_datagram的最大大小 in
  actual_size_pt 作为其名称 out
  skipped_byte_count_ptr 调用时无法识别的字节计数 out
  返回true表示成功，否则表示失败
*/
static bool get_raw_datagram_from_serial(
    uint8_t *restrict raw_datagram, size_t max_size,
    size_t *restrict actual_size_ptr, size_t *restrict skipped_byte_count_ptr)
{
    *skipped_byte_count_ptr = 0;
Top:
    // find the SOT
    for (;;) {
        uint8_t byte = 0;
        if (!uart_recv_byte(&byte, (uint32_t)(-1))) {
            // recv no longer OK
            return false;
        }

        if (byte == DATAGRAM_SOT) {
            break;
        }
        (*skipped_byte_count_ptr)++;
    }


    size_t curr_idx;

SoT_found:
    curr_idx = 0;
    raw_datagram[curr_idx++] = DATAGRAM_SOT;
    for(;; curr_idx++) {
        // store every byte recved until a EOT

        if (curr_idx + 1 >= max_size) {
            // no space to store
            (*skipped_byte_count_ptr) += curr_idx;
            goto Top;
        }

        if (!uart_recv_byte(raw_datagram + curr_idx, (uint32_t)(-1))) {
            // recv no longer OK
            return false;
        }

        if (raw_datagram[curr_idx] == DATAGRAM_EOT) {
            break;
        }

        if (raw_datagram[curr_idx] == DATAGRAM_SOT) {
            (*skipped_byte_count_ptr) += curr_idx;
            raw_datagram[curr_idx] = 0;
            goto SoT_found;
        }
    }

    *actual_size_ptr = curr_idx + 1;
    return true;
}

/*该函数将读取UART，获得一个接一个的数据报字符串，
   将字符串进行数据报文解码得到原始数据，在进行数
   据报PB解码获得的数据
*/
void serial_datagram_receive_loop(void *arg)
{
    (void)arg;
    for(;;) { // for each datagram
        // get raw-data
        char datagram_str[MAX_DATAGRAM_LEN];
        size_t datagram_str_size;
        uint8_t decoded_data[MAX_DATAGRAM_BUF_LEN];
        size_t decoded_data_len = sizeof decoded_data;
        size_t skipped_count;

        memset(datagram_str, 0, sizeof datagram_str);
        memset(decoded_data, 0, sizeof decoded_data);
        int ret = get_raw_datagram_from_serial( (uint8_t *)datagram_str, sizeof datagram_str, &datagram_str_size, &skipped_count);
        if (!ret) {
            return;
        }
        ret = decode_from_datagram(decoded_data,&decoded_data_len,datagram_str,datagram_str_size); // <数据报> -> 数据报data
        if(ret)
        {
            ret = process_incoming_datagram(decoded_data, decoded_data_len);//已经还原了发送端意图发给本端的数据，调用语义层处理函数完成随后处理
        }else{
            // TODO: 错误报文数登记或产生日志
        }
    } // for each datagram
}

__attribute__((weak)) bool process_incoming_datagram(const void *data_ptr, unsigned short len) {
    (void)data_ptr;
    (void)len;
    return false;
}

/* 发送编码后的数据报文
    data_ptr 是编码后的数据报文存放的指针，in
    len是编码后的数据报文存放的长度的指针，in/out
    返回true表示成功。失败通常是因为编码失败，以及发送失败
*/
bool send_datagram(const void *data_ptr, unsigned short len) {
    bool ret;
    if (len == 0) {
        return false;
    }
    uint8_t data_gram_buf[MAX_DATAGRAM_BUF_LEN] = {0};
    size_t datagram_len = sizeof data_gram_buf;
    ret = encode_to_datagram(data_gram_buf, &datagram_len, data_ptr, len);
    if (ret) {
        ret = uart_send_data(data_gram_buf, datagram_len, (uint32_t)(-1));
    }
    return ret;
}
