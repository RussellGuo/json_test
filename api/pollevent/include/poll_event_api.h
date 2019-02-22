
#ifndef POLL_EVENT_API_H
#define POLL_EVENT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

void PollEventInit(void);
void PollEventDeinit(void);

typedef enum {
    event_idx_timer = 0,
    event_idx_key,
    event_idx_touch,
    event_idx_bar_scan,
    event_idx_wifi,
    event_idx_4g,
    event_idx_tts,
    event_idx_reseved_0,
    event_idx_max = 31,
} event_idx_t;

bool PollEventSetFd(event_idx_t idx, int fd);
bool PollEventDectect(const bool event_vector[], event_idx_t vector_size);
bool HasKeyEvent(void);
bool HasBarScanEvent(void);
bool HasWiFiEvent(void);
bool Has4gEvent(void);
bool hasGenericEvent(event_idx_t idx);


#ifdef __cplusplus
} // extern "C"
#endif
#endif /*POLL_EVENT_API_H*/
