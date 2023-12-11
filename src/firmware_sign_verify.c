/*
    ARM MCU端针对固件的签名验证模块
    和 sign_verify 模块不同的是，这是一个特定的模块，针对固件的布局来做验证
    郭强(guoqiang@huaqin.com)
    2023-11-30 创建

    2023-12-09 签名方案修改为更先进的PSS

 */

#include "firmware_sign_verify.h"

#include "sign_verify.h"
#include "firmware_layout_constant.h"

// 依照规定的固件布局，从固件中提取“可执行体”的长度
// 逐字节拼接的，是可移植的写法
inline static size_t get_exec_len_inside_firmware_memory(const uint8_t *firmware_memory)
{
    size_t ret = 0;
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        ret += firmware_memory[EXEC_LEN_ADDR + i] << (8 * i);
    }
    return ret;
}

/*
    参数 firmw_memory 是固件起始地址
    参数 public_key_pem_string 是公钥的pem形式的字串
    返回true表示验证通过，反之反是
 */
bool firmware_sign_verify(const uint8_t *firmware_memory, const uint8_t *public_key_pem_string)
{
    // 先获得“可执行体”的长度
    size_t exec_len = get_exec_len_inside_firmware_memory(firmware_memory);
    if (exec_len >= EXEC_BODY_MAX_SIZE) { // 固件大小太大的话
        return false;
    }

    // 有了长度，就可以验证签名了
    bool ret = sign_verify_sha512_rsa2048_pkcs1_pss_padding(
        public_key_pem_string,                         // 公钥字串
        firmware_memory + BEGIN_POS_OF_PLAIN,          // 被签名数据的起始地址
        EXEC_BODY_POS + exec_len - BEGIN_POS_OF_PLAIN, // 被签名数据的长度
        firmware_memory + SIGNATURE_POS);              // 验证签名

    return ret;
}
