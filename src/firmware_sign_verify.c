#include "firmware_sign_verify.h"

#include "sign_verify.h"
#include "firmware_layout_constant.h"

inline static size_t get_exec_len_inside_firmware_memory(const uint8_t *firmware_memory)
{
    size_t ret = 0;
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        ret += firmware_memory[EXEC_LEN_ADDR + i] << (8 * i);
    }
    return ret;
}

bool firmware_sign_verify(const uint8_t *firmware_memory, const uint8_t *public_key_pem_string)
{
    size_t exec_len = get_exec_len_inside_firmware_memory(firmware_memory);
    if (exec_len >= EXEC_BLOCK_MAX_SIZE) { // 固件大小太大的话
        return false;
    }
    bool ret = sign_verify_sha512_rsa2048_pkcs1_padding(
        public_key_pem_string,
        firmware_memory + BEGIN_POS_OF_PLAIN, EXEC_BLOCK_POS + exec_len - BEGIN_POS_OF_PLAIN,
        firmware_memory + SIGNATURE_POS);  // 验证签名

    return ret;
}
