#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>
#include <memory>

#include "json.hpp"

size_t idx;
struct str {
    str() {
        std::cerr << "created " << n << "\n";
    }
    str(const str&s): map(s.map) {
        std::cerr << "duplicated " << n << "\n";
    }
    str(str&&s):map(std::move(s.map)){
        std::cerr << "moved " << n << "\n";
    }
    ~str() {
        std::cerr << "released " << n <<" len = " << map.size() << "\n";
    }
    const size_t n = idx++;
    std::map<const char *, int>map;
};

struct value_t {
    str s;
};

int main(int argc, char *argv[])
{
    str s1;
    s1.map["char"] = 1;
    s1.map["short"] = 2;
    auto p = std::make_shared<const value_t, value_t>({.s = std::move(s1)});
    p = nullptr;
    return 0;
}
