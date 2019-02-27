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

struct event_info_t {
    event_info_t(int _fd = -1, callback_t _callback = nullptr, uint64_t _arg = 0, bool _enabled = false):fd(_fd), callback(_callback), arg(_arg), enabled(_enabled)
    {}
    int fd = -1;
    callback_t callback = nullptr;
    uint64_t arg = 0;
    bool enabled = false;
    void fire() const {
        if (callback) {
            callback(fd, arg);
        }
    }
};

constexpr ssize_t MAX_COUNT_EVENT = 32;
event_info_t event_info_map[MAX_COUNT_EVENT];
ssize_t event_last_item_idx = -1;

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

static void inline debug_poll_event_tab(int ret, const char *func_name)
{
    printf("function: %s, ret = %d\r\n", func_name, ret);
    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        printf("event_info_map[%d] = %d\r\n", event_info_map[i].fd, event_info_map[i].enabled);
    }
}
extern "C" bool setPollEventFd(int fd, callback_t callback, uint64_t arg, bool enabled)
{
    if (fd <= 0) {
        return false;
    }
    if (event_last_item_idx >= MAX_COUNT_EVENT - 1 - 1) { // MAX_COUNT_EVENT - 1 is the last room, must has 1 room left.
        return false;
    }
    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        if (event_info_map[i].fd == fd) {
            fprintf(stderr, "fd is alreay in the event fd list\r\n");
            return false;
        }
    }
    event_info_map[++event_last_item_idx] = event_info_t(fd, callback, arg, enabled);
    debug_poll_event_tab(true, __FUNCTION__);
    return true;
}

extern "C" bool enablePollEventFd(int fd, bool enabled)
{
    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        if (event_info_map[i].fd == fd) {
            event_info_map[i].enabled = enabled;
            return true;
        }
    }
    return false;
}

extern "C" bool delPollEventFd(int fd)
{
    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        if (event_info_map[i].fd == fd) {
            fprintf(stderr, "SHOULD remove %d at[%zd]\r\n", fd, i);
            if (i != event_last_item_idx) { // not the last one, just move the last one here
                event_info_map[i] = event_info_map[event_last_item_idx];
            }
            --event_last_item_idx;
            debug_poll_event_tab(true, __FUNCTION__);
            return true;
        }
    }
    return false;
}


extern "C" int PollEventSpinOnce(void)
{
    pollfd poll_fd_tab[MAX_COUNT_EVENT];
    ssize_t count = 0;

    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        if (!event_info_map[i].enabled) {
            continue;
        }
        poll_fd_tab[count].fd      = event_info_map[i].fd;
        poll_fd_tab[count].events  = POLLIN;
        poll_fd_tab[count].revents = 0;
        count++;
    }
    auto ret = poll(poll_fd_tab, nfds_t(count), -1);
    for (int i = 0; i < count; i++) {
        if (poll_fd_tab[i].revents == 0) {
            continue;
        }
        int fd = poll_fd_tab[i].fd;
        for (ssize_t k = 0; k <= event_last_item_idx; k++) {
            if (event_info_map[k].fd == fd) {
                event_info_map[k].fire();
                break;
            }
        }
    }
    return ret;
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
