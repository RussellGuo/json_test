#ifndef __DATAGRAM_CODEC_H__
#define __DATAGRAM_CODEC_H__

#include <stdbool.h>
#include <stddef.h>

/*
    datagram_codec.h
    数据报编解码，郭强，2023-08-23

    本案中，对于原始数据字节序列 byte1 byte2 ... byten，数据报格式是：
        SOT  byte1转码 byte2转码 ... byten转码 crc8转码  EOT
    SOT EOT是指定的特殊字符，在原始字符序列中不太常见的。在二者之间不得出现SOT和EOT。如果出现了，则采用转码
    另一个转义符叫做ESC。byte（以及crc）是SOT/EOT/ESC时候，转码结果是 'ESC S' 'ESC E' 'ESC ESC'。ESC之后只能跟这三种
    这么安排是为了便于在UART这样的字节流数据链路上传送数据报，并在发生问题后能够重新找到新的报文
    CRC采用简单的crc-8

 */

#define MAX_DATAGRAM_LEN 600 //TPDO:数据报数据长度
#define MAX_DATAGRAM_BUF_LEN 1024 //TPDO:数据报编解码数据长度

// 数据报的特殊符号，见上面描述
#define DATAGRAM_SOT 0xAA
#define DATAGRAM_EOT 0xAB
#define DATAGRAM_ESC 0xAC
// 转义符后面跟随S/E分别代表 SOT/EOT
#define DATAGRAM_ALT_SOT 'S'
#define DATAGRAM_ALT_EOT 'E'

/* 针对原始数据形成数据报格式
    dst dst_size是编码后的数据存放的指针，in
    dst_size是编码后的数据存放的长度的指针，in/out
    src/src_size是编码前的数据存放的指针和长度  in
    返回true表示成功。失败通常是因为数据不够存放
*/
bool encode_to_datagram(void *restrict dst, size_t *dst_size, const void *restrict src, size_t src_size);

/* 针对数据报数据，还原原始数据格式
    dst dst_size是解码后的数据存放的指针，in
    dst_size是解码后的数据存放的长度的指针，in/out
    返回true表示成功。失败通常是因为数据不够存放，也可能是因为转义符转移失败
*/
bool decode_from_datagram(void *restrict dst, size_t *dst_size, const void *restrict src, size_t src_size);

/* 发送编码后的数据报文
    data_ptr 是编码后的数据报文存放的指针，in
    len是编码后的数据报文存放的长度的指针，in/out
    返回true表示成功。失败通常是因为编码失败，以及发送失败
*/
bool send_datagram(const void *data_ptr, unsigned short len);

/*该函数将读取UART，获得一个接一个的数据报字符串，
   将字符串进行数据报文解码得到原始数据，在进行数
   据报PB解码获得的数据
*/
void serial_datagram_receive_loop(void *arg);
#endif
