#include <assert.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "remote_message.pb.h"

int main(int, char *[]) {
    login_req req;
    req.set_username("Russell");
    req.set_password("12345");

    char req_buf[600] = {0};
    size_t req_buf_len;
    bool ret = req.SerializeToArray(req_buf, sizeof req_buf);
    if (!ret) {
        std::cerr << "protobuf error" << std::endl;
        exit(1);
    }
    req_buf_len = req.ByteSizeLong();
    std::cerr << "protobuf translated byte count: " << req_buf_len << std::endl;

    // pre-built buf here
    const char expected_buf[] = {0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    const bool encoding_result = memcmp(req_buf, expected_buf, sizeof expected_buf) == 0 && req_buf_len == sizeof expected_buf;
    std::cerr << "encoding result: " << encoding_result << std::endl;

    const char res_buf[] = {8, 2};
    login_res res;
    auto is_ok = res.ParseFromArray(res_buf, sizeof res_buf);
    if (!is_ok) {
        std::cerr << "protobuf decoding error" << std::endl;
        exit(1);
    }

    return 0;
}
