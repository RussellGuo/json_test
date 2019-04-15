#ifndef RELAY_PCM_PLAY_H
#define RELAY_PCM_PLAY_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

struct relay_pcm_func_t {
    void *user_data_ptr;
    void (*replay_pcm_begin)(void *user_data_ptr);
    bool (*relay_pcm_feed)(void *user_data_ptr, const void *buf, unsigned size);
    bool (*relay_pcm_end)(void *user_data_ptr);
};

void set_relay_pcm_callback_func(const struct relay_pcm_func_t *_relay_pcm_func);


void replay_pcm_begin(void);
bool relay_pcm_feed(const void *buf, unsigned size);
bool relay_pcm_end(void);

#if defined(__cplusplus)
};
#endif

#endif // RELAY_PCM_PLAY_H
