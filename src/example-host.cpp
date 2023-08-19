#include <assert.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "remote_message.pb.h"

// C++ protobuf示例。郭强，2023-7-25

// 一个总的编码函数，针对pb对象，生成其二进制数据（流化）
template <typename T, size_t MAX_SIZE = 600>
std::vector<uint8_t> pb_encoding(const T &pb_obj) {
    unsigned char buf[MAX_SIZE] = {0};                    // 预计的size
    bool ret = pb_obj.SerializeToArray(buf, sizeof buf);  // 重点函数
    if (ret) {
        auto req_buf_len = pb_obj.ByteSizeLong();
        return std::vector<uint8_t>(buf, buf + req_buf_len);  // 转换之
    } else {
        return std::vector<uint8_t>();  // 出错，返回空
    }
}

// 解码示例，传入bye序列，输出到pb_obj.
// 输入：byte_data, 编码后的二进制数据
// 输出：pb_obj, 解码后的数据存放处
// 返回值：true代表正确
template <typename T>
bool pb_decoding(const std::vector<uint8_t> &byte_data, T &pb_obj) {
    // 创建流
    google::protobuf::io::CodedInputStream stream(byte_data.data(), byte_data.size());
    // 解码
    auto is_ok = pb_obj.ParseFromCodedStream(&stream);
    // 还要判断一下是否都吃掉了（我们假设输入数据单纯是pb_obj的，没有尾巴，没有多余字段
    auto pos = stream.CurrentPosition();
    auto all_bytes_consumed = size_t(pos) == byte_data.size();
    // 俩条件都要满足
    return is_ok && all_bytes_consumed;
}

int main(int, char *[]) {
    // 这是固定用法，检查.so库和 .exe 是否匹配
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // 准备请求数据
    login_req login_req_obj;
    login_req_obj.set_username("Russell");
    login_req_obj.set_password("12345");
    // 获得串行化数据
    auto login_req_obj_binary = pb_encoding(login_req_obj);
    if (login_req_obj_binary.empty()) {
        // error
        std::cerr << "login_req encoding error" << std::endl;
        exit(1);
    }
    std::cerr << "login_req: protobuf translated byte count: " << login_req_obj_binary.size() << std::endl;

    // 应该当是这个结果。注意这个串会拿到MCU端解码。目前这部分是我手工复制过去的。
    const std::vector<uint8_t> expected_login_encoding_binary{0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    std::cerr << "login_req: encoding result: " << (login_req_obj_binary == expected_login_encoding_binary) << std::endl;

    //
    // 解码示例，login_res编码后就俩字节
    const std::vector<uint8_t> login_res_buf{8, 2};
    login_res login_res_obj;
    auto is_ok = pb_decoding(login_res_buf, login_res_obj);
    if (!is_ok) {
        std::cerr << "login_req: protobuf decoding error" << std::endl;
        exit(1);
    }
    std::cerr << "login_res.status() is " << login_res_obj.status() << std::endl;

    // 示例oneof的用法
    to_mcu to_mcu_req_obj;
    to_mcu_req_obj.set_allocated_login(&login_req_obj);  // 这个写法是我找到的唯一能用的方法
    to_mcu_req_obj.set_seq(0x1U);
    auto to_mcu_req_binary = pb_encoding(to_mcu_req_obj);
    if (to_mcu_req_binary.empty()) {
        // error
        std::cerr << "to_mcu_req_obj encoding error" << std::endl;
        exit(1);
    }
    to_mcu_req_obj.release_login();  // 如果不是new出来的指针，得尽快release掉，否则会在to_mcu_req的析构中被灾难性的delete. 此处的release不会调用delete
    std::cerr << "to_mcu_req: protobuf translated byte count: " << to_mcu_req_binary.size() << std::endl;

    // 应该当是这个结果。注意这个串会拿到MCU端解码。目前这部分是我手工复制过去的。
    const std::vector<uint8_t> expected_to_mcu_req_encoding_binary{0x08, 0x01, 0x1a, 0x10, 0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    std::cerr << "to_mcu_req encoding result: " << (to_mcu_req_binary == expected_to_mcu_req_encoding_binary) << std::endl;

    // 解码from_mcu
    // 跟普通解码没有不同。不同的是获取obj的值的方法要注意
    const std::vector<uint8_t>from_mcu_res_binary {0x08, 0x02, 0x10, 0x04, 0x1a, 0x02, 0x08, 0x02};
    from_mcu from_mcu_res_obj;
    is_ok = pb_decoding(from_mcu_res_binary, from_mcu_res_obj);
    is_ok &= from_mcu_res_obj.res_case() == from_mcu_res_obj.kLogin;
    is_ok &= from_mcu_res_obj.login().status() == boolean_t::unknown;
    is_ok &= from_mcu_res_obj.err_code() == remote_call_err_code::no_impl;
    std::cerr << "from_mcu_res: decoding result = " << is_ok << std::endl;

    return 0;
}
