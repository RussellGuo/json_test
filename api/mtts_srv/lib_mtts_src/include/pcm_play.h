#ifndef PCM_PLAY_H
#define PCM_PLAY_H

#if defined(__cplusplus)
extern "C" {
#endif

void pcm_prepair(void);

void pcm_begin(void);
void pcm_feed(const void *buf, unsigned size);
void pcm_end(void);
#if defined(__cplusplus)
};
#endif

#endif // PCM_PLAY_H
