#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>

#include <linux/input.h>

#include "poll_event_api.h"
#include <pthread.h>

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
    void clean() {
        fd = -1;
        callback = nullptr;
        arg = 0;
        enabled = false;
    }
};

constexpr ssize_t MAX_COUNT_EVENT = 32;
event_info_t event_info_map[MAX_COUNT_EVENT];
ssize_t event_last_item_idx = -1;

static pthread_mutex_t mutex_polling = PTHREAD_MUTEX_INITIALIZER;

class auto_lock_t {
public:
    auto_lock_t() { pthread_mutex_lock(&mutex_polling);}
    ~auto_lock_t() { pthread_mutex_unlock(&mutex_polling);}
};
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
    auto_lock_t autolock;
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
    auto_lock_t autolock;
    for (ssize_t i = 0; i <= event_last_item_idx; i++) {
        if (event_info_map[i].fd == fd) {
            fprintf(stderr, "SHOULD remove %d at[%zd]\r\n", fd, i);
            if (i != event_last_item_idx) { // not the last one, just move the last one here
                event_info_map[i] = event_info_map[event_last_item_idx];
                event_info_map[event_last_item_idx].clean();
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
    {
        auto_lock_t autolock;

        for (ssize_t i = 0; i <= event_last_item_idx; i++) {
            if (!event_info_map[i].enabled) {
                continue;
            }
            poll_fd_tab[count].fd      = event_info_map[i].fd;
            poll_fd_tab[count].events  = POLLIN;
            poll_fd_tab[count].revents = 0;
            count++;
        }
    }
    if (count <= 0) {
        return false;
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
