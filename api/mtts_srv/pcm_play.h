#ifndef PCM_PLAY_H
#define PCM_PLAY_H

#include <stdbool.h>

#define BUZZER_PCM_SAMPLE_RATE 48000
void pcm_prepair(void);

void pcm_begin(unsigned msec);
bool pcm_feed(const void *buf, unsigned size);
bool pcm_end(void);

void pcm_abort(void);


#endif // PCM_PLAY_H
