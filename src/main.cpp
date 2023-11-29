#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "firmware_layout_constant.h"
#include "gen_signature.hpp"

uint8_t firmware_memory[TOTAL_FIRMWARE_MAX_SIZE];

void print_usage(const char *argv0) {
    fprintf(stderr, "Usage: %s -r private-key.pem -u public-key.pem -o output.bin -e executable.bin\n", argv0);
    exit(1);
}

size_t load_execable_into_firmware_memory(const char *exec_filename) {
    FILE *f = fopen(exec_filename, "rb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open execable file '%s' to read\n", exec_filename);
        exit(1);
    }
    auto read_bytes = fread(firmware_memory + EXEC_BLOCK_POS, 1, EXEC_BLOCK_MAX_SIZE, f);
    bool is_ok = !ferror(f);
    is_ok &= feof(f);
    is_ok &= read_bytes < EXEC_BLOCK_MAX_SIZE;
    fclose(f);
    if (!is_ok) {
        fprintf(stderr, "reading execable file '%s' failed\n", exec_filename);
        exit(1);
    }
    return read_bytes;
}

void set_exec_len_into_firmware_memory(size_t size) {
    if (size >= EXEC_BLOCK_MAX_SIZE) {
        fprintf(stderr, "executable file size is too big");
        exit(1);
    }
    // 小端存储，逐字节处理，而没有用32位整型的内存指针，是为了可移植性
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        firmware_memory[EXEC_LEN_ADDR + i] = (size >> (8 * i)) & 0xFF;
    }
}

size_t get_exec_len_inside_firmware_memory() {
    size_t ret = 0;
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        ret += firmware_memory[EXEC_LEN_ADDR + i] << (8 * i);
    }
    return ret;
}

bool save_firmware_memory_into_file(const char *output_filename) {
    FILE *f = fopen(output_filename, "wb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open file '%s' for output", output_filename);
    }
    bool is_ok;
    const auto should_written = EXEC_BLOCK_POS + get_exec_len_inside_firmware_memory();
    size_t written = fwrite(firmware_memory, 1, EXEC_BLOCK_POS + get_exec_len_inside_firmware_memory(), f);
    is_ok = written == should_written;
    is_ok &= fclose(f) == 0;
    return is_ok;
}

bool save_firmware_memory_into_inc_file(const char *inc_output_filename) {
    FILE *f = fopen(inc_output_filename, "wb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open file '%s' for inc output", inc_output_filename);
    }

    // 打印签名数据的C数组形式。为的是复制后贴入MCU端看那边解码是否正确
    const auto last_pos = EXEC_BLOCK_POS + get_exec_len_inside_firmware_memory();
    for (size_t i = 0; i < last_pos; i++) {
        fprintf(f, "%s", i % 16 == 0 ? "    " : " ");
        fprintf(f, "0x%02X,", firmware_memory[i]);
        if (i % 16 == 15) {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "\n");
    bool is_ok = !ferror(f);
    is_ok &= fclose(f) == 0;
    return is_ok;
}

// 目前主程序就是一个示例。对 '1234'这个字串签名并验证。
int main(int argc, char *argv[]) {
    const char *private_key_pem_filename = nullptr;
    const char *public_key_pem_filename = nullptr;
    const char *exec_bin_filename = nullptr;
    const char *output_firmware_filename = nullptr;
    bool save_inc_file = false;

    auto test_and_assign_opt_string_to_variable = [argv](const char *&variable) {
        if (variable != nullptr) {
            print_usage(argv[0]);
        }
        variable = optarg;
    };

    for (;;) {
        int opt = getopt(argc, argv, "r:u:o:e:i");
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

            case 'i':
                save_inc_file = true;
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

    auto exec_len = load_execable_into_firmware_memory(exec_bin_filename);
    set_exec_len_into_firmware_memory(exec_len);

    bool ret;
    ret = sign_by_sha512_rsa2048_pkcs1_padding(
        private_key_pem_filename,
        firmware_memory + BEGIN_POS_OF_PLAIN, EXEC_BLOCK_POS + exec_len - BEGIN_POS_OF_PLAIN,
        firmware_memory + SIGNATURE_POS);  // 签名
    if (!ret) {
        fprintf(stderr, "signing signature failed\n");
        exit(1);
    }
    fprintf(stderr, "signing signature: OK\n");
    auto exec_len_inside_firmware_memory = get_exec_len_inside_firmware_memory();
    assert(exec_len_inside_firmware_memory == exec_len);

    ret = verify_sign_by_sha512_rsa2048_pkcs1_padding(
        public_key_pem_filename,
        firmware_memory + BEGIN_POS_OF_PLAIN, EXEC_BLOCK_POS + exec_len_inside_firmware_memory - BEGIN_POS_OF_PLAIN,
        firmware_memory + SIGNATURE_POS);  // 验证签名
    if (!ret) {
        fprintf(stderr, "signature verification failed\n");
        exit(1);
    }
    fprintf(stderr, "signature verification: OK\n");

    ret = save_firmware_memory_into_file(output_firmware_filename);
    if (!ret) {
        fprintf(stderr, "saving firmware failed\n");
        exit(1);
    }
    fprintf(stderr, "saving firmware: OK\n");

    if (save_inc_file) {
        const std::string inc_filename = std::string(output_firmware_filename) + ".inc";
        ret = save_firmware_memory_into_inc_file(inc_filename.c_str());
        if (!ret) {
            fprintf(stderr, "saving firmware.inc failed\n");
            exit(1);
        }
        fprintf(stderr, "saving firmware.inc: OK\n");
    }

    return 0;
}
