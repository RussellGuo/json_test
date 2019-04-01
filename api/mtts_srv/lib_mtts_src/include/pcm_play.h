#ifndef PCM_PLAY_H
#define PCM_PLAY_H

void pcm_prepair(void);

void pcm_begin(void);
void pcm_feed(const void *buf, unsigned size);
void pcm_end(void);

#endif // PCM_PLAY_H
