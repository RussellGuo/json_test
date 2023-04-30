
#include <iostream>

#include "json.hpp"
#include "time_used.hxx"

using json = nlohmann::json;

static bool string_only_in_json;
#include "2038json.inc"

constexpr int times = 300;

int main() {
    for (auto string_only_flag : {false, true}) {
        string_only_in_json = string_only_flag;
        std::cout << (string_only_in_json ? "has no number:" : "has number:") << std::endl;
        time_used_t gen_many_times_json("gen_many_times_json");
        for (int i = 0; i < times; i++) {
            json main_json = MAIN_JSON();
        }
        gen_many_times_json.print();

        json main_json = MAIN_JSON();
        time_used_t dump_many_times_json("dump_many_times_json");
        for (int i = 0; i < times; i++) {
            auto string = main_json.dump();
        }
        gen_many_times_json.print();
    }

    return 0;
}
