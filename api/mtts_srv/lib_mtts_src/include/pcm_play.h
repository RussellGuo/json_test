#ifndef PCM_PLAY_H
#define PCM_PLAY_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void pcm_prepair(void);

bool pcm_local_buzzer_play(uint16_t freq, uint16_t msec, uint16_t volume);
void pcm_begin(void);
void pcm_feed(const void *buf, unsigned size);
void pcm_end(void);
#if defined(__cplusplus)
};
#endif

#endif // PCM_PLAY_H
