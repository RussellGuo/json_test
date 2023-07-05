#include <iostream>
#include <string>
#include <vector>

#include "obj_hash.hpp"

// 自定义类型的hash函数怎样做的例子
class person_t {
   public:
    enum gender_t : char {
        male,
        female,
        others,
    };

    int no;
    const char *name;
    gender_t gender;
    std::string address;
    struct score {
        int mid;
        int fin;
    };

    std::vector<score> score_list;
};

inline void feed_hash_with(obj_hash_t &h, const person_t::score &score) {
    feed_hash_with(h, score.mid);
    feed_hash_with(h, score.fin);
    std::cout << "score hash..." << std::endl;
}

inline void feed_hash_with(obj_hash_t &h, const person_t &person) {           // 固定用这个接口
    feed_hash_with(h, person.no);                                             // 顺序*投喂*基本类型的数据
    feed_hash_with(h, person.name);                                           // 顺序*投喂*C字符串
    feed_hash_with(h, person.gender);                                         // 枚举也是可以的
    feed_hash_with(h, person.address);                                        // 顺序*投喂*C++字符串
    feed_hash_with(h, person.score_list.cbegin(), person.score_list.cend());  // vector 则用迭代器顺序投喂
    feed_hash_with(h, person.score_list);  // vector 则用迭代器顺序投喂
    feed_hash_with(h, L"AAA");
    // 如果成员里是其它自定义类型，也是这么写，前提是那个自定义类型已经实现了feed_hash_with
}

int main(int, char *[]) {
    person_t person{
        .no = 1,
        .name = "Li Lei",
        .gender = person_t::male,
        .address = "#399, Chuanhe ave",
        .score_list{
            {90, 114},
            {90, 104},
            {90, 99},
        }};                                       // 对象
    auto hash_value = get_hash_value_of(person);  // 简便方法获得其hash
    (void)hash_value;
    return 0;
}
