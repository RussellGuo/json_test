#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <poll.h>

#include <linux/input.h>

#include "timer_api.h"
#include "poll_event_api.h"

static void timer_callback(timer_id_t id, uint64_t arg)
{
    uint64_t count;
    auto ret = read(id, &count, sizeof count);
    if (ret == sizeof count) {
        auto proc = reinterpret_cast<timer_callback_t>(arg);
        proc(id, count);
    }
}

extern "C" timer_id_t createTimer(clockid_t clockid, int flags,  const itimerspec *itimerspec, timer_callback_t callback, bool enabled)
{
    timer_id_t id = -1;
    bool registered = false;
    bool set_timer_succeed = false;
    id = timerfd_create(clockid, TFD_NONBLOCK | TFD_CLOEXEC);
    if (id <= 0) {
        perror("createTimer");
        fprintf(stderr,"\r\n");
        goto error;
    }
    registered = setPollEventFd(id, timer_callback, reinterpret_cast<uint64_t>(callback), enabled);
    if (!registered) {
        goto error;
    }

    set_timer_succeed = modifyTimer(id, flags, itimerspec);
    if (!set_timer_succeed) {
        goto error;
    }

    return id;

error:
    if (id >= 0) {
        close(id);
    }
    if (registered) {
        delPollEventFd(id);
    }
    return -1;
}

static void set_itimerspec_by_simple_params(itimerspec *itimerspec, uint32_t msec, bool one_shot)
{
    itimerspec->it_value.tv_sec  = msec / 1000;
    itimerspec->it_value.tv_nsec = (msec % 1000) * 1000000; // one ms means 10**6 nano second
    itimerspec->it_interval.tv_sec  = one_shot ? 0 : itimerspec->it_value.tv_sec;
    itimerspec->it_interval.tv_nsec = one_shot ? 0 : itimerspec->it_value.tv_nsec;
}
extern "C" timer_id_t createSimpleTimer(uint32_t msec, bool one_shot, timer_callback_t callback)
{
    //
    itimerspec itimerspec;
    set_itimerspec_by_simple_params(&itimerspec, msec, one_shot);
    timer_id_t id = createTimer(CLOCK_MONOTONIC, 0, &itimerspec, callback, true);
    return id;
}

extern "C" bool modifySimpleTimer(timer_id_t timer_id, uint32_t msec, bool one_shot)
{
    //
    itimerspec itimerspec;
    set_itimerspec_by_simple_params(&itimerspec, msec, one_shot);
    auto ret = modifyTimer(timer_id, 0, &itimerspec);
    return ret;
}

extern "C" bool modifyTimer(timer_id_t timer_id, int flags, const struct itimerspec *itimerspec)
{
    int ret = timerfd_settime(timer_id, flags, itimerspec, nullptr);
    if (ret < 0) {
        perror("modifyTimer");
        fprintf(stderr,"\r\n");
    }
    return ret == 0;
}

extern "C" bool enableTimer(timer_id_t timer_id, bool enabled)
{
    auto ret = enablePollEventFd(timer_id, enabled);
    return ret;
}

extern "C" bool delTimer(timer_id_t timer_id)
{
    int ret = delPollEventFd(timer_id);
    if (ret) {
        close(timer_id);
    }
    return ret;
}
