#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>
#include <memory>
#include <set>

#include "json.hpp"

struct value_t {
    const int value;
    const std::string name;
    value_t(int v, const std::string &_name): value(v), name(_name) {}
};

static inline bool operator < (const value_t &v1, const value_t &v2) {
    bool ret = v1.value < v2.value;
    return ret;
}
static inline bool operator < (const value_t &v1, int v2) {
    bool ret = v1.value < v2;
    return ret;
}
static inline bool operator < (int v1, const value_t &v2) {
    bool ret = v1 < v2.value;
    return ret;
}

using value_set_t = std::set<value_t, std::less<>>;

int main(int argc, char *argv[])
{
    value_set_t v_set;
    const auto r1 = v_set.emplace(1, "A");
    const auto r2 = v_set.emplace(2, "B");
    const auto r3 = v_set.emplace(2, "A");
    const auto r4 = v_set.find(2);
    const auto r5 = v_set.find(4);
    const auto r6 = r5 == v_set.end();


    return 0;
}
