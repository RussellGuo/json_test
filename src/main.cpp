#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "gen_signature.hpp"

constexpr size_t TOTAL_FIRMWARE_MAX_SIZE = 2 * 1024 * 1024;           // 整体firmware的最大尺寸
constexpr size_t SIGNATURE_POS = 0;                                   // 签名的位置
constexpr size_t SIGNATURE_SIZE = RSA2048_RESULT_LEN;                 // 签名的大小
constexpr size_t EXEC_LEN_ADDR = SIGNATURE_POS + SIGNATURE_SIZE + 0;  // 可执行体的大小，放在firmware的这个偏移地址上
constexpr size_t EXEC_LEN_SIZE = 4;                                   // 长度是4字节。little-endian。
constexpr size_t EXEC_BLOCK_POS = 512;                                // MCU的bin文件起始地址
constexpr size_t EXEC_BLOCK_MAX_SIZE = TOTAL_FIRMWARE_MAX_SIZE - EXEC_BLOCK_POS;

uint8_t firmware_memory[TOTAL_FIRMWARE_MAX_SIZE];

void print_usage(const char *argv0) {
    fprintf(stderr, "Usage: %s -r private-key.pem -u public-key.pem -o output.bin -e executable.bin\n", argv0);
    exit(1);
}

// 目前主程序就是一个示例。对 '1234'这个字串签名并验证。
int main(int argc, char *argv[]) {
    const char *private_key_pem_filename = nullptr;
    const char *public_key_pem_filename = nullptr;
    const char *exec_bin_filename = nullptr;
    const char *output_firmware_filename = nullptr;

    auto test_and_assign_opt_string_to_variable = [argv](const char *&variable) {
        if (variable != nullptr) {
            print_usage(argv[0]);
        }
        variable = optarg;
    };
    for (;;) {
        int opt = getopt(argc, argv, "r:u:o:e:");
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case 'r':  // private_key_pem_filename
                test_and_assign_opt_string_to_variable(private_key_pem_filename);
                break;
            case 'u':  // public_key_pem_filename
                test_and_assign_opt_string_to_variable(public_key_pem_filename);
                break;
            case 'o':  // output_firmware_filename
                test_and_assign_opt_string_to_variable(output_firmware_filename);
                break;
            case 'e':  // exec_bin_filename
                test_and_assign_opt_string_to_variable(exec_bin_filename);
                break;

            default: /* '?' */
                print_usage(argv[0]);
        }
    }
    if (optind != argc ||
        private_key_pem_filename == nullptr ||
        public_key_pem_filename == nullptr ||
        output_firmware_filename == nullptr ||
        exec_bin_filename == nullptr) {
        print_usage(argv[0]);
    }

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
