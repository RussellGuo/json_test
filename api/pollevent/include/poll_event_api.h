
#ifndef POLL_EVENT_API_H
#define POLL_EVENT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

bool PollEventInit(void);
bool PollEventDeinit(void);

bool PollEventDectect(bool key_event, bool bar_scan_event, bool net_event, bool timer_event);
bool PollEventHasKeyEvent(void);
bool PollEventHasBarScanEvent(void);
bool PollEventHasNetEvent(void);
bool PollEventHasTimerEvent(void);


#ifdef __cplusplus
} // extern "C"
#endif
#endif /*POLL_EVENT_API_H*/
