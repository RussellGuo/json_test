
#ifndef TIMER_API_H
#define TIMER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/timerfd.h>

typedef int timer_id_t;
typedef void (*timer_callback_t)(timer_id_t id);
timer_id_t createTimer(clockid_t clockid, int flags, const struct itimerspec *itimerspec, timer_callback_t callback, bool enabled);
bool modifyTimer(timer_id_t timer_id, int flags, const struct itimerspec *itimerspec);
bool enableTimer(timer_id_t timer_id, bool enabled);
bool delTimer(timer_id_t timer_id);



#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TIMER_API_H*/
