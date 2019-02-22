#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "timer_api.h"


static timer_node *timerlist = NULL;
static struct timeval mainloop_timeout;
static struct timeval current_time;

static void calculate_timeval(struct timeval *tv, msec_t to); 
static int32_t time_to_expiry(struct timeval *t);

/**
 * Create a new one-shot timer.
 *
 * @param timeout number of milliseconds before the timer should activate
 * @param callback Callback function to call when timer fires.
 * @param arg Opaque argument to pass to callback function.
 * @return Timer handle.  NOTE that this is automatically destroyed
 * after the callback function has been called.
 */
timer_node *AddTimer(msec_t timeout, timer_callback_t callback, void *arg)
{
    timer_node *newtimer;

    if (timeout <= 0) {
        return NULL;
    }
    if(!(newtimer = malloc(sizeof(timer_node)))) return NULL;

    gettimeofday(&current_time, NULL);

    if(timerlist) timerlist->prev = newtimer;

    calculate_timeval(&newtimer->timeout, timeout);
    newtimer->callback = callback;
    newtimer->arg      = arg;
    newtimer->next     = timerlist;
    newtimer->prev     = NULL;
    newtimer->type     = timer_type_oneshot;
    newtimer->period   = timeout;

    timerlist = newtimer;

    return newtimer;
}

/**
 * Create a new periodic (repeating) timer.
 *
 * @param timeout number of milliseconds between activations.
 * @param callback Callback function to call when timer fires.
 * @param arg Opaque argument to pass to callback function.
 * @return Timer handle.
 */
timer_node *AddPeriodicTimer(msec_t timeout, timer_callback_t callback, void *arg)
{
    timer_node *newtimer = AddTimer(timeout, callback, arg);
    if (newtimer) {
        newtimer->type = timer_type_periodic;
    }
    return newtimer;
}

/**
 * Destroy a timer.
 *
 * @param timer Timer to destroy.
 */
void DestroyTimer(timer_node *timer)
{
    if(timer->next) timer->next->prev = timer->prev;
    if(timer->prev) timer->prev->next = timer->next;
    if(timer == timerlist) {
        if(timer->next) timerlist = timer->next;
        else timerlist = timer->prev;
    }
    free(timer);
}


/**
 * Check timer list for pending timeout
 *
 * @param tv
 * @param timeout
 * @return
 */
bool GetNextTimeout(struct timeval *tv, msec_t timeout)
{
    int32_t i, lowest_timeout;
    timer_node *t = timerlist;

    if(!timeout && !timerlist) return false;

    gettimeofday(&current_time, NULL);

    if(timeout) {
        calculate_timeval(&mainloop_timeout, timeout);
        lowest_timeout = time_to_expiry(&mainloop_timeout);
    } else {
        lowest_timeout = time_to_expiry(&t->timeout);
        mainloop_timeout.tv_sec = -1;
        t = t->next;
    }

    while(t) {
        i = time_to_expiry(&t->timeout);
        if( i>0 && i>t->period ) {
            calculate_timeval (&t->timeout, t->period);
            i = t->period;
        }
        if(i < lowest_timeout) lowest_timeout = i;
        t = t->next;
    }

    if(lowest_timeout <= 0) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    } else {
        tv->tv_sec = lowest_timeout / 1000;
        tv->tv_usec = (lowest_timeout % 1000) * 1000;
    }

    return true;
}

/**
 * Check timer list for pending timeout
 */
bool Timeout(void)
{
    timer_node *n, *t = timerlist;
    int32_t tempTimeout = 0;

    gettimeofday(&current_time, NULL);

    while(t) {
        n = t->next;
        tempTimeout = time_to_expiry(&t->timeout);
        if( tempTimeout<= 0) {
            t->callback(t->arg);
            if (t->type == timer_type_oneshot)
                DestroyTimer(t); 		/* One shot timer, is finished delete it now */
            else
                calculate_timeval (&t->timeout, t->period); /* Periodic timer needs to be reset */
        } else {
            if( tempTimeout>0 && tempTimeout>t->period )
                calculate_timeval (&t->timeout, t->period);
        }
        t = n;
    }

    if(mainloop_timeout.tv_sec > 0 || mainloop_timeout.tv_usec > 0)
        if(time_to_expiry(&mainloop_timeout) <= 0)
            return true;

    return false;
}

static void calculate_timeval(struct timeval *tv, msec_t to)
{
    tv->tv_sec = current_time.tv_sec + (to / 1000);
    tv->tv_usec = current_time.tv_usec + ((to % 1000) * 1000);
    if(tv->tv_usec > 1000000) {
        tv->tv_sec++;
        tv->tv_usec -= 1000000;
    }
}

static int32_t time_to_expiry(struct timeval *t)
{
    long ret = (((t->tv_sec - current_time.tv_sec) * 1000) +
                ((t->tv_usec - current_time.tv_usec) / 1000));

    return (int32_t)ret;
}

