#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>

#include <linux/input.h>

#include "poll_event_api.h"

static struct pollfd poll_fd_tab[event_idx_max];

void PollEventInit(void)
{
    for (event_idx_t i = 0; i < event_idx_max; i++) {
        poll_fd_tab[i].fd = -1;
    }
}

bool PollEventSetFd(event_idx_t idx, int fd)
{
    if (idx < event_idx_max) {
        poll_fd_tab[idx].fd = fd;
        return true;
    } else {
        return false;
    }
}
void PollEventDeinit(void)
{
}

bool PollEventDectect(const bool event_vector[], event_idx_t vector_size)
{
    if (vector_size >= event_idx_max) {
        return false;
    }
    for (event_idx_t i = 0; i < event_idx_max; i++) {
        poll_fd_tab[i].events = i < vector_size && event_vector[i] ? POLLIN : 0;
    }
    int ret = poll(poll_fd_tab, event_idx_max, -1);
    return ret > 0;
}

bool hasGenericEvent(event_idx_t idx)
{
    if (idx >= event_idx_max) {
        return false;
    }
    return poll_fd_tab[idx].revents != 0;
}
bool HasKeyEvent(void)
{
    bool ret = hasGenericEvent(event_idx_key);
    return ret;
}

bool HasBarScanEvent(void)
{
    bool ret = hasGenericEvent(event_idx_bar_scan);
    return ret;
}

bool HasWiFiEvent(void)
{
    bool ret = hasGenericEvent(event_idx_wifi);
    return ret;
}

bool Has4gEvent(void)
{
    bool ret = hasGenericEvent(event_idx_4g);
    return ret;
}
