#include <assert.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "json.hpp"
#include "time_used.hxx"
#include "txt_num.pb.h"

constexpr int TIMES = 1000000;

using json = nlohmann::json;
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

    time_used_t pb("protobuf");
    for (int i = 0; i < TIMES; i++) {
        txt_num_tab txt_num_tab_pb;
        for (const auto &[txt, num] : txt_num_table) {
            auto item = txt_num_tab_pb.add_tab();
            item->set_txt(txt);
            item->set_num(num);
        }
        char buf[600] = {0};
        bool ret = txt_num_tab_pb.SerializeToArray(buf, sizeof buf);
        if (!ret) {
            fprintf(stderr, "protobuf error\n");
            exit(1);
        }
        static bool printed;
        if (!printed) {
            std::cout << "protobuf translated byte count: " << txt_num_tab_pb.ByteSizeLong() << std::endl;
            printed = true;
        }
    }
    pb.print();

    time_used_t json_dump("json_dump");
    for (int i = 0; i < TIMES; i++) {
        json j_tab = "[]"_json;
        for (const auto &[txt, num] : txt_num_table) {
            json j;
            j["txt"] = txt;
            j["num"] = num;
            j_tab.push_back((j));
        }
        auto s = j_tab.dump();

        static bool printed;
        if (!printed) {
            std::cout << s << std::endl;
            printed = true;
        }
    }
    json_dump.print();

    time_used_t str_cat("str_cat");
    for (int i = 0; i < TIMES; i++) {
        char buf[600] = {0};
        size_t l = 0;
        l += sprintf(buf, "[");
        for (const auto &[txt, num] : txt_num_table) {
            l += sprintf(buf + l, R"({"num":%lf,"txt":"%s"},)", num, txt.c_str());
        }
        sprintf(buf + l - 1, "]");

        static bool printed;
        if (!printed) {
            fprintf(stderr, "%s\n", buf);
            printed = true;
        }
    }
    str_cat.print();

    return 0;
}
