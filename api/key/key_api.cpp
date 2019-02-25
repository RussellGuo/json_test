#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>
#include <dirent.h>
#include <sys/poll.h>

#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "key_api.h"
#include "poll_event_api.h"

namespace {
key_event_callback_t key_event_callback;
int key_fd_tab[] = { -1, -1};

void key_callback(int dev_fd, uint64_t arg)
{
    struct input_event event;
    auto ret = read(dev_fd, &event, sizeof event);
    // fprintf(stderr, "ret, type, code, value: %d %u %u %d\r\n", ret, event.type, event.code, event.value);
    if (ret == sizeof event && event.type == EV_KEY && key_event_callback != nullptr) {
        key_event_callback(event.code, event.value);
    } else if (ret < 0) {
        perror("read key");
        fprintf(stderr, "\r\n");
    }
}

}


extern "C" bool initKeyEvent(key_event_callback_t key_event)
{
    if (key_fd_tab[0] >= 0 || key_fd_tab[1] >= 0) {
        fprintf(stderr, "key event already initialized\r\n");
        return false;
    }

    int i = 0;
    const char *dev_name_tab[] = {"/dev/input/event0", "/dev/input/event2" };
    for (auto dev_name:dev_name_tab) {
        int dev_fd = open(dev_name, 0);
        if (dev_fd > 0) {
            key_fd_tab[i++] = dev_fd;
            setPollEventFd(dev_fd, key_callback, 0, true);
        } else {
            perror("open key device");
            fprintf(stderr, "\r\n");
        }
    }

    auto ret = i > 0;
    if (ret) {
        key_event_callback = key_event;
    }
    return ret;
}

extern "C" bool deinitKeyEvent(void)
{
    key_event_callback = nullptr;
    for (auto &fd:key_fd_tab) {
        delPollEventFd(fd);
        close(fd);
        fd = -1;
    }
    return true;
}
