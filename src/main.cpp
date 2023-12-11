/*
    加密机端签名程序
    郭强 2023-11-30
    签名程序是针对Keil（或者其它开发工具）生成的二进制“可执行体”加入签名等附加信息的程序。详情请见print_usage的描述
 */
#include <assert.h>
#include <getopt.h>
#include <stdio.h>

#include <fstream>
#include <iostream>

#include "firmware_layout_constant.h"
#include "gen_signature.hpp"

// 最终生成的fireware的内存平面数据
static uint8_t firmware_memory[TOTAL_FIRMWARE_MAX_SIZE];

/*
    打印程序用法并退出，参数是本程序的可执行程序名。命令行选项中
    -r 带出 RSA2048 的私钥，pem形式的文件。这个文件十分当紧，绝不可泄露
    -u 带出 RSA2048 的共钥，pem形式的文件。这个文件是要共享给MCU端的。本程序会用这个公钥验证签名
    -o 带出 输出的固件的二进制文件名，这个就是发行用的文件
    -e 带出 Keil或其它“可执行体”的二进制形式。此文件要在Keil中用fromaxf生成（fromelf --bin --output=outfile.bin infile.axf）
    -i 选项可选可不选，如果加上，还会生成一个 .inc输出文件，这个文件可以直接复制到MCU端以源程序的形式被装在MCU程序里。这么做是为了调试
 */
static void print_usage(const char *argv0) {
    fprintf(stderr, "Usage: %s -r private-key.pem -u public-key.pem -o output.bin -e executable.bin [-i]\n", argv0);
    fprintf(stderr, "  -i means to generate .inc file for the whole firmware, then let MCU program to test\n");
    exit(1);
}

/*
    装入“可执行体”，参数名字就是bin的名字。返回值是bin的大小。出错直接退出
    装入的地方在前面的firmware_memory里

 */
static size_t load_execable_into_firmware_memory(const char *exec_filename) {
    FILE *f = fopen(exec_filename, "rb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open execable file '%s' to read\n", exec_filename);
        exit(1);
    }
    auto read_bytes = fread(firmware_memory + EXEC_BODY_POS, 1, EXEC_BODY_MAX_SIZE, f);
    bool is_ok = !ferror(f);
    is_ok &= feof(f);
    is_ok &= read_bytes < EXEC_BODY_MAX_SIZE;
    fclose(f);
    if (!is_ok) {
        fprintf(stderr, "reading execable file '%s' failed\n", exec_filename);
        exit(1);
    }
    return read_bytes;
}

/*
    把“可执行体”的长度按照布局填入firmware_memory中。出错直接退出
    布局中采用小端存放，可移植的写法
 */
static void set_exec_len_into_firmware_memory(size_t size) {
    if (size >= EXEC_BODY_MAX_SIZE) {
        fprintf(stderr, "executable file size is too big");
        exit(1);
    }
    // 小端存储，逐字节处理，而没有用32位整型的内存指针，是为了可移植性
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        firmware_memory[EXEC_LEN_ADDR + i] = (size >> (8 * i)) & 0xFF;
    }
}

// 从firmware_memory中读出“可执行体”的长度，并返回
// 逐字节读出并拼出来，可移植的写法
static size_t get_exec_len_inside_firmware_memory() {
    size_t ret = 0;
    for (size_t i = 0; i < EXEC_LEN_SIZE; i++) {
        ret += firmware_memory[EXEC_LEN_ADDR + i] << (8 * i);
    }
    return ret;
}

// 把firmware_memory写出到指定的文件中
// 正确返回true，反之反是
static bool save_firmware_memory_into_file(const char *output_filename) {
    FILE *f = fopen(output_filename, "wb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open file '%s' for output", output_filename);
    }
    bool is_ok;
    const auto should_written = EXEC_BODY_POS + get_exec_len_inside_firmware_memory();                      // 读出大小
    size_t written = fwrite(firmware_memory, 1, EXEC_BODY_POS + get_exec_len_inside_firmware_memory(), f);  // 写文件
    is_ok = written == should_written;                                                                      // 写全了没？
    is_ok &= !ferror(f);                                                                                    // 出错了没？
    is_ok &= fclose(f) == 0;                                                                                // 出错了没？

    return is_ok;
}

/*
    把firmware内容打印到指定的文件中，格式是便于直接被C/C++程序#include到程序里的
    正确返回true，反之反是
    这么做是因为最开始的时候固件传输还没做好，这个数据可以直接被MCU端包含，就可以测试这部分了。
 */
static bool save_firmware_memory_into_inc_file(const char *inc_output_filename) {
    FILE *f = fopen(inc_output_filename, "wb");
    if (f == nullptr) {
        fprintf(stderr, "cannot open file '%s' for inc output", inc_output_filename);
    }

    // 打印签名数据的C数组形式。为的是复制后贴入MCU端看那边解码是否正确
    const auto last_pos = EXEC_BODY_POS + get_exec_len_inside_firmware_memory();
    for (size_t i = 0; i < last_pos; i++) {
        fprintf(f, "%s", i % 16 == 0 ? "    " : " ");  // 往下都是格式控制
        fprintf(f, "0x%02X,", firmware_memory[i]);
        if (i % 16 == 15) {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "\n");
    bool is_ok = !ferror(f);
    is_ok &= fclose(f) == 0;  // 各种检查
    return is_ok;
}

int main(int argc, char *argv[]) {
    // 按照用法读入各种选项，存入变量
    const char *private_key_pem_filename = nullptr;
    const char *public_key_pem_filename = nullptr;
    const char *exec_bin_filename = nullptr;
    const char *output_firmware_filename = nullptr;
    bool save_inc_file = false;

    // 保险起见，这些变量不应该存两次，所以带检查
    auto test_and_assign_opt_string_to_variable = [argv](const char *&variable) {
        if (variable != nullptr) {
            print_usage(argv[0]);
        }
        variable = optarg;
    };

    // 标准套路，获得选项变量
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

    // 是不是都有了？
    if (optind != argc ||
        private_key_pem_filename == nullptr ||
        public_key_pem_filename == nullptr ||
        output_firmware_filename == nullptr ||
        exec_bin_filename == nullptr) {
        print_usage(argv[0]);
    }

    // 开始，先装入“可执行体”
    auto exec_len = load_execable_into_firmware_memory(exec_bin_filename);
    // 再存入长度
    set_exec_len_into_firmware_memory(exec_len);

    // 对指定区域做摘要以及签名
    bool ret;
    ret = sign_by_sha512_rsa2048_pkcs1_pss_padding(
        private_key_pem_filename,
        firmware_memory + BEGIN_POS_OF_PLAIN, EXEC_BODY_POS + exec_len - BEGIN_POS_OF_PLAIN,
        firmware_memory + SIGNATURE_POS);  // 签名
    if (!ret) {
        fprintf(stderr, "signing signature failed\n");
        exit(1);
    }
    fprintf(stderr, "signing signature: OK\n");

    // 读出写入的“可执行体”长度
    auto exec_len_inside_firmware_memory = get_exec_len_inside_firmware_memory();
    assert(exec_len_inside_firmware_memory == exec_len);

    // 用公钥验证签名
    ret = verify_sign_by_sha512_rsa2048_pkcs1_pss_padding(
        public_key_pem_filename,
        firmware_memory + BEGIN_POS_OF_PLAIN, EXEC_BODY_POS + exec_len_inside_firmware_memory - BEGIN_POS_OF_PLAIN,
        firmware_memory + SIGNATURE_POS);  // 验证签名
    if (!ret) {
        fprintf(stderr, "signature verification failed\n");
        exit(1);
    }
    fprintf(stderr, "signature verification: OK\n");

    // 保存固件
    ret = save_firmware_memory_into_file(output_firmware_filename);
    if (!ret) {
        fprintf(stderr, "saving firmware failed\n");
        exit(1);
    }
    fprintf(stderr, "saving firmware: OK\n");

    // 可选的保存C格式固件
    if (save_inc_file) {
        const std::string inc_filename = std::string(output_firmware_filename) + ".inc"; // 文件名咱也不传入了，直接加一个 .inc 了事
        ret = save_firmware_memory_into_inc_file(inc_filename.c_str());
        if (!ret) {
            fprintf(stderr, "saving firmware.inc failed\n");
            exit(1);
        }
        fprintf(stderr, "saving firmware.inc: OK\n");
    }

    return 0;
}
