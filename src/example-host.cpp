#include <assert.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "txt_num.pb.h"

int main(int, char *[]) {
    struct txt_num_t {
        const std::string txt;
        const double num;
    };
    std::vector<txt_num_t> txt_num_table{
        {"zero", 0},
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"night", 9},
    };

    txt_num_tab txt_num_tab_to_pb;
    for (const auto &[txt, num] : txt_num_table) {
        auto item = txt_num_tab_to_pb.add_tab();
        item->set_txt(txt);
        item->set_num(num);
    }
    char buf[600] = {0};
    size_t buf_content_len;
    bool ret = txt_num_tab_to_pb.SerializeToArray(buf, sizeof buf);
    if (!ret) {
        fprintf(stderr, "protobuf error\n");
        exit(1);
    }
    buf_content_len = txt_num_tab_to_pb.ByteSizeLong();
    std::cout << "protobuf translated byte count: " << buf_content_len << std::endl;

    txt_num_tab pb_to_txt_num_tab;
    auto is_ok = pb_to_txt_num_tab.ParseFromArray(buf, buf_content_len);
    if (!is_ok) {
        fprintf(stderr, "protobuf decoding error\n");
        exit(1);
    }
    auto size = pb_to_txt_num_tab.tab_size();
    for (int i = 0; i < size; i++) {
        const auto &item = pb_to_txt_num_tab.tab(i);
        const auto txt = item.txt();
        const auto num = item.num();
        std::cout << "txt: " << txt << " num: " << num << std::endl;
    }

    return 0;
}
