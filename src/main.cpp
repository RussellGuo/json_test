#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>

using binary_t = std::vector<uint8_t>;

class thread_safe_pipe_t {
   public:
    explicit thread_safe_pipe_t(size_t _max_len = 16U * 1024) : max_len(_max_len), p(new uint8_t[max_len]) {}
    ~thread_safe_pipe_t() { delete[] p; }

    thread_safe_pipe_t(const thread_safe_pipe_t &) = delete;
    thread_safe_pipe_t(thread_safe_pipe_t &&) = delete;
    thread_safe_pipe_t &operator=(const thread_safe_pipe_t &) = delete;
    thread_safe_pipe_t &operator=(thread_safe_pipe_t &&) = delete;

    bool queue_data(const uint8_t *data_ptr, uint32_t data_len) {
        const auto new_tail_idx = tail_idx + data_len;
        if (new_tail_idx > max_len) {
            return false;
        }
        {
            std::lock_guard<std::mutex> lk(cv_m);
            memcpy(p + tail_idx, data_ptr, data_len);
            tail_idx = new_tail_idx;
        }
        cv.notify_one();

        return true;
    }

    void queue_no_more_data() {
        {
            std::lock_guard<std::mutex> lk(cv_m);
            finished = true;
        }
        cv.notify_one();
    }

    void dequeue_data(const uint8_t *&data_ptr, size_t &data_num, size_t data_item_len) {
        {
            std::unique_lock<std::mutex> lk(cv_m);
            cv.wait(lk, [this, data_item_len] { return finished || data_left() >= data_item_len; });
            auto const count = (tail_idx - head_idx) / data_item_len;
            data_num = count;
            data_ptr = p + head_idx;
            head_idx += count * data_item_len;
        }
    }
    size_t data_left() const {
        return tail_idx - head_idx;
    }

    void reset() {
        head_idx = tail_idx = 0;
        finished = false;
    }

   private:
    std::condition_variable cv{};
    std::mutex cv_m{};
    const size_t max_len;
    uint8_t *const p;
    size_t head_idx = 0, tail_idx = 0;
    bool finished{};
};

int main(int, char *[]) {
    thread_safe_pipe_t pipe{32};

    auto consumer_proc = [&pipe] {
        for (;;) {
            size_t count;
            const uint8_t *data_start;
            pipe.dequeue_data(data_start, count, 4);
            if (count == 0) {
                break;
            }
        }
    };
    std::thread t{consumer_proc};
    uint8_t buf[10];
    memcpy(buf, "1234", 4);
    pipe.queue_data(buf, 4);
    memcpy(buf, "abcd", 4);
    usleep(10000);
    pipe.queue_data(buf, 4);
    pipe.queue_no_more_data();
    t.join();
    return 0;
}
