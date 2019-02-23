#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>

#include <linux/input.h>

#include "poll_event_api.h"

#include <map>
#include <vector>

namespace {

struct event_info_t {
    event_info_t(){}
    event_info_t(int _fd, callback_t _callback, uint64_t _arg, bool _enabled):fd(_fd), callback(_callback), arg(_arg), enabled(_enabled)
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

std::map<int, event_info_t> event_info_map;

}

static void inline debug_poll_event_tab(int ret, const char *func_name)
{
    printf("function: %s, ret = %d\r\n", func_name, ret);
    for (auto n:event_info_map) {
        printf("event_info_map[%d] = %d\r\n", n.first, n.second.enabled);
    }
}
extern "C" bool setPollEventFd(int fd, callback_t callback, uint64_t arg, bool enabled)
{
    if (fd <= 0) {
        return false;
    }
    event_info_t info(fd, callback, arg, enabled);
    auto ret = event_info_map.insert(std::make_pair(fd, info)).second;
    debug_poll_event_tab(ret, __FUNCTION__);
    return ret;
}

extern "C" bool enablePollEventFd(int fd, bool enabled)
{
    auto event = event_info_map.find(fd);
    bool ret = event != event_info_map.end();
    if (ret) {
        event->second.enabled = enabled;
    }
    debug_poll_event_tab(ret, __FUNCTION__);
    return ret;
}

extern "C" bool delPollEventFd(int fd)
{
    auto ret = int(event_info_map.erase(fd));
    debug_poll_event_tab(ret, __FUNCTION__);
    return ret > 0;
}


extern "C" int PollEventSpinOnce(void)
{
    std::vector<pollfd>poll_fd_tab;
    for (auto n:event_info_map) {
        auto &event = n.second;
        if (event.enabled) {
            pollfd p;
            p.fd = event.fd;
            p.events = POLLIN;
            p.revents = 0;
            poll_fd_tab.push_back(p);
        }
    }

    if (poll_fd_tab.size() <= 0) {
        return false;
    }
    auto ret = poll(poll_fd_tab.data(), poll_fd_tab.size(), -1);
    for (auto const &pollfd:poll_fd_tab) {
        if (pollfd.revents) {
            event_info_map[pollfd.fd].fire();
        }
    }
    return ret;
}
