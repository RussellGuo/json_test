#ifndef TIMER_API_H
#define TIMER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

typedef int32_t msec_t;

typedef void (*timer_callback_t)(void *);

typedef enum {
    timer_type_oneshot,
    timer_type_periodic,
} timer_type_t;

typedef struct timer_node {
    struct timeval timeout;
    timer_callback_t callback;
    void *arg;
    timer_type_t type;
    msec_t period;

    struct timer_node *next, *prev;
} timer_node;

timer_node *AddTimer(msec_t timeout, timer_callback_t callback, void *arg);
timer_node *AddPeriodicTimer(msec_t timeout, timer_callback_t callback, void *arg);
void     DestroyTimer(timer_node *timer);
bool     GetNextTimeout(struct timeval *tv, msec_t timeout);
bool     Timeout(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TIMER_API_H*/
