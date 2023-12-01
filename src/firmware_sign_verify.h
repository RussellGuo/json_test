#ifndef __FIRMWARE_SIGN_VERIFY_H__
#define __FIRMWARE_SIGN_VERIFY_H__

/*
    ARM MCU端针对固件的签名验证模块
    和 sign_verify 模块不同的是，这是一个特定的模块，针对固件的布局来做验证
    郭强(guoqiang@huaqin.com)
    2023-11-30 创建

 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
    参数 firmw_memory 是固件起始地址
    参数 public_key_pem_string 是公钥的pem形式的字串
    返回true表示验证通过，反之反是
 */
bool firmware_sign_verify(const uint8_t *firmware_memory, const uint8_t *public_key_pem_string);

#endif
