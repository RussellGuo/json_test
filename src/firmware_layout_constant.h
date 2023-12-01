#ifndef __FIRMWARE_LAYOUT_CONSTANT_H__
#define __FIRMWARE_LAYOUT_CONSTANT_H__

/*
    固件布局所用的常数
    郭强 2023-11-30
    在本案中，我们初步定下来固件的布局。整个固件最大尺寸暂定为2M。
    签名的二进制数据放置在固件的0地址，长度位256个字节。
    随后跟着一个4字节长度的“可执行体”的长度数据。这是Keil编译出来生成的.bin（注意不是hex）的长度
    中间跳掉252字节，这252字节可以留作其它字段使用，比如版本号、版本信息等自定义内容
    从512字节开始存放的就是“可执行体”本身了。
    前面说的签名，针对的数据是包含了从4字节开始到执行体末尾的全部数据。也就是说长度信息、附加信息、可执行体均受签名保护
    被签名的数据先经过SHA512算法变成固定64字节长度的摘要，摘要则通过RSA算法、指定的私钥来签出。保存在固件开头
 */

#define TOTAL_FIRMWARE_MAX_SIZE (2 * 1024 * 1024)                           // 整体firmware的最大尺寸
#define SIGNATURE_POS           0                                           // 签名的位置
#define SIGNATURE_SIZE          256                                         // 签名的大小，就是RSA2048的数据输入输出大小
#define EXEC_LEN_ADDR           (SIGNATURE_POS + SIGNATURE_SIZE + 0)        // 可执行体的大小，放在firmware的这个偏移地址上
#define EXEC_LEN_SIZE           4                                           // 长度是4字节。little-endian。
#define EXEC_BODY_POS          512                                          // MCU的bin文件起始地址
#define EXEC_BODY_MAX_SIZE     (TOTAL_FIRMWARE_MAX_SIZE - EXEC_BODY_POS)    // 执行体的最大尺寸
#define BEGIN_POS_OF_PLAIN      EXEC_LEN_ADDR                               // 签名起始位置。 结束位置在EXEC_BODY_POS + exec长度

#endif
