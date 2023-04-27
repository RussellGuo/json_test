#ifndef __TIME_USED__
#define __TIME_USED__

#include <chrono>
#include <stdint.h>
#include <stdio.h>

#include <string>

class time_used_t {
    
public:
    typedef int64_t time_t;
    time_used_t(const char *_name):name(_name), begin_time(time_ns()) {
    }
    time_t used_time() const {
        auto curr = time_ns();
        auto ret = curr - begin_time;
        return ret;
    }
    static time_t time_ns() {
        using namespace std::chrono;
        system_clock::time_point tp = system_clock::now();
        system_clock::duration dtn = tp.time_since_epoch();
        return dtn.count();
    }
    void print() const {
        auto v = used_time();
        printf("<TU>%s: %ld ms\n", name.c_str(), v / 1000000);
    }

private:
    std::string name;
    const time_t begin_time;
};

#endif
