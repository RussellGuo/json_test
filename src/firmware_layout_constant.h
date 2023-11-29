#ifndef __FIRMWARE_LAYOUT_CONSTANT_H__
#define __FIRMWARE_LAYOUT_CONSTANT_H__

#define TOTAL_FIRMWARE_MAX_SIZE (2 * 1024 * 1024)                           // 整体firmware的最大尺寸
#define SIGNATURE_POS           0                                           // 签名的位置
#define SIGNATURE_SIZE          256                                         // 签名的大小，就是RSA2048的数据输入输出大小
#define EXEC_LEN_ADDR           (SIGNATURE_POS + SIGNATURE_SIZE + 0)        // 可执行体的大小，放在firmware的这个偏移地址上
#define EXEC_LEN_SIZE           4                                           // 长度是4字节。little-endian。
#define EXEC_BLOCK_POS          512                                         // MCU的bin文件起始地址
#define EXEC_BLOCK_MAX_SIZE     (TOTAL_FIRMWARE_MAX_SIZE - EXEC_BLOCK_POS)  // 执行体的最大尺寸
#define BEGIN_POS_OF_PLAIN      EXEC_LEN_ADDR                               // 签名起始位置。 结束位置在EXEC_BLOCK_POS + exec长度

#endif
