#include <assert.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "remote_message.pb.h"

// C++ protobuf示例。郭强，2023-7-25

// 编码示例，输入login_req，输出byte序列。出错时候序列为空
inline static std::vector<uint8_t> login_req_encoding(const login_req &req) {
    unsigned char req_buf[600] = {0};                          // 预计的size，一般不会超过
    bool ret = req.SerializeToArray(req_buf, sizeof req_buf);  // 重点函数
    if (ret) {
        auto req_buf_len = req.ByteSizeLong();
        return std::vector<uint8_t>(req_buf, req_buf + req_buf_len);  // 转换之
    } else {
        return std::vector<uint8_t>();  // 出错，返回空
    }
}

// 解码示例，传入bye序列，输出login_req.
// 输入：byte_data, 编码后的二进制数据
// 输出：res, 解码后的数据存放处
// 返回值：true代表正确
inline static bool login_res_decoding(const std::vector<uint8_t> &byte_data, login_res &res) {
    // 创建流
    google::protobuf::io::CodedInputStream stream(byte_data.data(), byte_data.size());
    // 解码
    auto is_ok = res.ParseFromCodedStream(&stream);
    // 还要判断一下是否都吃掉了（我们假设输入数据单纯是login_res的，没有尾巴，没有多余字段
    auto pos = stream.CurrentPosition();
    auto all_bytes_consumed = size_t(pos) == byte_data.size();
    // 俩条件都要满足
    return is_ok && all_bytes_consumed;
}

int main(int, char *[]) {
    // 这是固定用法，检查.so库和 .exe 是否匹配
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // 准备请求数据
    login_req req;
    req.set_username("Russell");
    req.set_password("12345");
    // 获得串行化数据
    auto login_req_data = login_req_encoding(req);
    if (login_req_data.empty()) {
        // error
        std::cerr << "login_in_req encoding error" << std::endl;
        exit(1);
    }
    std::cerr << "protobuf translated byte count: " << login_req_data.size() << std::endl;

    // 应该当是这个结果。注意这个串会拿到MCU端解码。目前这部分是我手工复制过去的。
    const std::vector<uint8_t> expected_encoding_result{0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    std::cerr << "encoding result: " << (login_req_data == expected_encoding_result) << std::endl;

    //
    // 解码示例，res编码后就俩字节
    const std::vector<uint8_t> res_buf{8, 2};
    login_res res;
    auto is_ok = login_res_decoding(res_buf, res);
    if (!is_ok) {
        std::cerr << "protobuf decoding error" << std::endl;
        exit(1);
    }
    std::cerr << "login_res.status() is " << res.status() << std::endl;

    return 0;
}
