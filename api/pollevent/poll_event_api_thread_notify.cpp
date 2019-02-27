#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>

#include <linux/input.h>

#include "poll_event_api.h"

namespace {

class thread_notify_t {
public:
    thread_notify_t() {
        int ret = pipe(thread_notify_pipe);
        if (ret < 0) {
            perror("init_thread_notify_pipe");
            fprintf(stderr, "\r\n");
            return;
        }
        setPollEventFd(thread_notify_pipe[0], thread_notify_callback, reinterpret_cast<uint64_t>(this), true);
    }
    ~thread_notify_t() {
        delPollEventFd(thread_notify_pipe[0]);
        close(thread_notify_pipe[0]);
        close(thread_notify_pipe[1]);
    }
    uint64_t fetch_thread_notify_count(uint64_t *last_thread_notify_value_ptr)
    {
        if (last_thread_notify_value_ptr) {
            *last_thread_notify_value_ptr = last_thread_notify_value;
        }
       auto ret = thread_notify_count_from_last_fetch;
       thread_notify_count_from_last_fetch = 0;
       return ret;
    }

    bool thread_notify(uint64_t arg) const
    {
        auto ret = write(thread_notify_pipe[1], &arg, sizeof arg);
        if (ret != sizeof arg) {
            perror("init_thread_notify_pipe");
            fprintf(stderr, "\r\n");
            return false;
        }
        return true;
    }
private:

    int thread_notify_pipe[2] = { -1, -1};

    uint64_t thread_notify_count_from_last_fetch = 0;
    uint64_t last_thread_notify_value;

    static void thread_notify_callback(int fd, uint64_t self_uint64)
    {
        thread_notify_t *self = reinterpret_cast<thread_notify_t *>(self_uint64);
        auto ret = read(fd, &self->last_thread_notify_value, sizeof self->last_thread_notify_value);
        if (ret != sizeof self->last_thread_notify_value) {
            perror("thread_notify_callback");
            fprintf(stderr, "\r\n");
            return;
        }
        self->thread_notify_count_from_last_fetch++;
    }

} thread_notify;

}

extern "C" uint64_t PollEventFetchThreadNotifyCount(uint64_t *last_thread_notify_value_ptr)
{
    auto ret = thread_notify.fetch_thread_notify_count(last_thread_notify_value_ptr);
   return ret;
}

extern "C" bool PollEventThreadNotify(uint64_t arg)
{
    auto ret = thread_notify.thread_notify(arg);
    return ret;
}
