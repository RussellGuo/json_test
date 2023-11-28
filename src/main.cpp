#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "gen_signature.hpp"

// 目前主程序就是一个示例。对 '1234'这个字串签名并验证。
int main(int, char *[]) {
    uint8_t sign_data[RSA2048_RESULT_LEN] = {0};
    auto ret = sign_by_sha512_rsa2048_pkcs1_padding("financial.pem", (const uint8_t *)"1234", 4, sign_data);  // 签名
    if (ret) {
        ret = verify_sign_by_sha512_rsa2048_pkcs1_padding("financial_pub.pem", (const uint8_t *)"1234", 4, sign_data);  // 验证签名
    }

    if (ret) {
        for (int i = 0; i < 256; i++) {  // 打印签名数据的C数组形式。为的是复制后贴入MCU端看那边解码是否正确
            fprintf(stderr, "    0x%02X,", sign_data[i]);
            if (i % 16 == 15) {
                fprintf(stderr, "\n");
            }
        }
    }

    printf("sign/verify return %d\n", ret);
    return ret ? 0 : 1;
}
