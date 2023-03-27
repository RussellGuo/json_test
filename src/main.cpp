#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>
#include <memory>
#include <set>

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

namespace const_str_pool {
using const_str = const char *;

struct const_str_less_t {
    bool operator () (const_str a, const_str b) const {
        auto ret = strcmp(a, b);
        return ret < 0;
    }
};

std::set<const_str, const_str_less_t>pool;
const_str mk_str(const_str str)
{
    auto ret = pool.find(str);
    if (ret == pool.end()) {
        const_str new_str = strdup(str);
        if (new_str == nullptr) {
            throw std::bad_alloc();
        }
        pool.insert(new_str);
        return new_str;
    } else {
        return *ret;
    }

}

};

int main(int, char *[])
{
    auto r1  = const_str_pool::mk_str("1");
    auto r2  = const_str_pool::mk_str("2");
    auto r1_ = const_str_pool::mk_str("1");
    auto r2_ = const_str_pool::mk_str("2");
    (void)r1;
    (void)r2;
    (void)r1_;
    (void)r2_;
    
    return 0;
}
