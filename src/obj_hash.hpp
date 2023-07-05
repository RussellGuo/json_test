#ifndef __OBJ_HASH_HPP__
#define __OBJ_HASH_HPP__

#include <assert.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <stdint.h>
#include <string.h>

#include <array>
#include <string>
#include <type_traits>

// obj_hash_t是针对对象获得其SHA1哈希值的底层类型，实现了如下基本类型的hash
//    整数、浮点数和枚举类型
//    C风格字符串和C++风格的字符串
//    一般的二进制数据段（这个是所有hash计算的最底层函数）
// 一个本对象，可以反复地使用上述基本类型的数据得到hash值。hash结果可以通过result成员获得
// 如果要从头来过，执行reset即可。之前feed的数据全部作废。这样一个对象可以反复使用
// 对于复合类型，参见后面的namespace obj_hash的函数组，实际上对于基础类型也是主张用obj_hash的，而不是直接用本类的

class obj_hash_t {
   public:
    using hash_result_t = std::array<uint8_t, SHA_DIGEST_LENGTH>;
    obj_hash_t() {
        reset();
    }

    void feed(const void *data_ptr, size_t len) {
        SHA1_Update(&ctx, data_ptr, len);
    }

    hash_result_t result() {
        hash_result_t res;
        SHA1_Final(res.data(), &ctx);
        return res;
    }

    void reset() {
        SHA1_Init(&ctx);
    }

   private:
    SHA_CTX ctx;
};

// 本namespace是实现基础类型和复合类型的hash的接口。所有的类型都拥有 feed_hash_with()函数，这是最重要的函数组
// 对于普通类型，直接用 feed_hash_with(h, v)
// 二进制类型用 feed_hash_with(h, data_ptr, len)
// 对于容器类，则使用 feed_hash_with(h, v.cbegin(), v.cend())
//    顺序容器是安全的，如vector list forward_list, dqueue, array
//    基于树的set map是安全的，multiset multimap目前未知，请谨慎使用
//    注意 unordered_set unordered_map系列不可以用，因为hash跟顺序是有关的
// 对于用户定义的复合类型，需要用户*伴随*定义自己的 feed_hash_with. 参见main的例子

static inline void feed_hash_with(obj_hash_t &h, const void *data_ptr, size_t len) {
    h.feed(data_ptr, len);
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value> >
static inline void feed_hash_with(obj_hash_t &h, const T value) {
    feed_hash_with(h, &value, sizeof value);
}


static inline void feed_hash_with(obj_hash_t &h, const std::string &str) {
    feed_hash_with(h, str.data(), str.length());
}

static inline void feed_hash_with(obj_hash_t &h, const char *str) {
    feed_hash_with(h, str, strlen(str));
}

template <typename T_const_iterator, typename = typename std::iterator_traits<T_const_iterator>::difference_type>
static inline void feed_hash_with(obj_hash_t &h, T_const_iterator begin, T_const_iterator end) {
    for (auto it = begin; it != end; it++) {
        feed_hash_with(h, *it);
    }
}

template <typename T_container, typename = typename T_container::difference_type>
static inline void feed_hash_with(obj_hash_t &h, const T_container &container) {
    for (const auto &item : container) {
        feed_hash_with(h, item);
    }
}

// 最后，有一个方便的接口 get_hash_value_of
template <class T>
static inline auto get_hash_value_of(const T &obj) {
    obj_hash_t h;
    feed_hash_with(h, obj);
    return h.result();
}

#endif
