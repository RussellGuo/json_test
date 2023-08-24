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

#include "datagram_codec.h"

#include <stdint.h>

#include "crc8_soft.h"

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
bool encode_to_datagram(void *restrict dst, size_t *dst_size, const void *restrict src, size_t src_size) {
    uint8_t *const dst_byte_ptr = dst;             // 字节类型方便使用
    size_t dst_curr_len = 0;                       // 变化中的输出长度
    const size_t dst_max = *dst_size;              // 保存获取的最大值
    const uint8_t crc = crc8_soft(src, src_size);  // TODO: 真实的crc需要填入

// 因为'输出字符'会被反复调用，并且带出错检查，所以弄个宏
// 带范围检查的存入dst的宏
#define PUT_BYTE(b)                         \
    if (dst_curr_len + 1 < dst_max) {       \
        dst_byte_ptr[dst_curr_len++] = (b); \
    } else                                  \
        return false

    PUT_BYTE(DATAGRAM_SOT);  // 报文开始

    for (size_t i = 0; i < src_size + 1; i++) {                               // 加的1是crc
        const uint8_t byte = i < src_size ? ((const uint8_t *)src)[i] : crc;  // 输出的字符比原始字符多一个crc。放在这里统一处理了
        switch (byte) {
            case DATAGRAM_SOT:  // 特殊字符，走特殊转码序列
                PUT_BYTE(DATAGRAM_ESC);
                PUT_BYTE(DATAGRAM_ALT_SOT);
                break;

            case DATAGRAM_EOT:  // 特殊字符，走特殊转码序列
                PUT_BYTE(DATAGRAM_ESC);
                PUT_BYTE(DATAGRAM_ALT_EOT);
                break;

            case DATAGRAM_ESC:  // 特殊字符，走特殊转码序列
                PUT_BYTE(DATAGRAM_ESC);
                PUT_BYTE(DATAGRAM_ESC);
                break;

            default:
                PUT_BYTE(byte);  // 不是特殊字符，不用转
                break;
        }
    }

    PUT_BYTE(DATAGRAM_EOT);  // 报文结束

#undef PUT_BYTE

    *dst_size = dst_curr_len;  // 传回真实长度
    return true;
}

/* 针对数据报数据，还原原始数据格式
    dst dst_size是解码后的数据存放的指针，in
    dst_size是解码后的数据存放的长度的指针，in/out
    返回true表示成功。失败通常是因为数据不够存放，也可能是因为转义符转移失败
*/
bool decode_from_datagram(void *restrict dst, size_t *restrict dst_size, const void *restrict src, size_t src_size) {
    uint8_t *const dst_byte_ptr = dst;  // 字节类型方便使用
    size_t dst_curr_len = 0;            // 变化中的输出长度
    const size_t dst_max = *dst_size;   // 保存获取的最大值

// 带范围检查的存入dst的宏
#define PUT_BYTE(b)                         \
    if (dst_curr_len + 1 < dst_max) {       \
        dst_byte_ptr[dst_curr_len++] = (b); \
    } else                                  \
        return false

    const uint8_t *src_byte_ptr = src;  // 字节类型方便使用
    size_t src_byte_left = src_size;

    if (src_byte_left < 2 || src_byte_ptr[0] != DATAGRAM_SOT || src_byte_ptr[src_byte_left - 1] != DATAGRAM_EOT) {  // 头尾得有，否则判错
        return false;
    }
    src_byte_ptr++;      // 头尾去掉
    src_byte_left -= 2;  //

// 从头到尾扫描输入字节序列所需的两个辅助宏，用于判断是否结尾
#define HAS_SRC_BYTE() (src_byte_left != 0)
// 以及是否能读出一个字节到变量b
#define GET_BYTE(b)            \
    if (HAS_SRC_BYTE()) {      \
        (b) = *src_byte_ptr++; \
        src_byte_left--;       \
    } else                     \
        return false

    // 扫描剩下的字符，解码后存放。同时CRC也会检查（总是最后一个）
    for (;;) {
        uint8_t src_byte, target_byte;  // 计划取一个字节，存一个字节

        GET_BYTE(src_byte);  // 取
        switch (src_byte) {
            case DATAGRAM_SOT:  // 特殊字符不该出现
            case DATAGRAM_EOT:  //
                return false;   //

            case DATAGRAM_ESC:                       // 转义字符处理
                GET_BYTE(src_byte);                  // 得读下一个
                switch (src_byte) {                  // 分情况
                    case DATAGRAM_ALT_SOT:           // SOT处理
                        target_byte = DATAGRAM_SOT;  //
                        break;

                    case DATAGRAM_ALT_EOT:           // EOT处理
                        target_byte = DATAGRAM_EOT;  //
                        break;
                    case DATAGRAM_ESC:               // ESC处理
                        target_byte = DATAGRAM_ESC;  //
                        break;

                    default:  // 其余的字节值都不对
                        return false;
                }
                break;

            default:
                target_byte = src_byte;  // 普通字节，直接用
                break;
        }

        if (HAS_SRC_BYTE()) {  // 看是不是还不是最后一个字节
            // 还没到最后一个，需要保存
            PUT_BYTE(target_byte);
        } else {
            // 最后一个了，检查crc
            // TODO: check CRC
            const uint8_t crc = crc8_soft(dst, dst_curr_len);
            if (target_byte != crc) {
                return false;
            }
            break;  // 然后退出循环
        }
    }

#undef PUT_BYTE
#undef GET_BYTE
#undef HAS_SRC_BYTE

    *dst_size = dst_curr_len;  // 保存长度
    return true;
}

#include <stdio.h>

int main(void) {
    uint8_t datagram_buf[100] = {0};
    size_t datagram_len;
    uint8_t raw_buf[100] = {0xaa, 'a', 0xab, 0xac};
    size_t raw_len;

    bool encoding_ret, decoding_ret;

    datagram_len = sizeof datagram_buf;
    encoding_ret = encode_to_datagram(datagram_buf, &datagram_len, raw_buf, 3);
    raw_len = sizeof raw_buf;
    decoding_ret = decode_from_datagram(raw_buf, &raw_len, datagram_buf, datagram_len);
    (void)encoding_ret;
    (void)decoding_ret;

    return 0;
}
