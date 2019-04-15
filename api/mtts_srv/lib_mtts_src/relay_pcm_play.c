#include <stdio.h>

#include "relay_pcm_play.h"

static struct relay_pcm_func_t relay_pcm_func;

void set_relay_pcm_callback_func(const struct relay_pcm_func_t *_relay_pcm_func) {
    relay_pcm_func = *_relay_pcm_func;
}

void replay_pcm_begin(void)
{
    if (relay_pcm_func.replay_pcm_begin == NULL) {
        return;
    }
    relay_pcm_func.replay_pcm_begin(relay_pcm_func.user_data_ptr);
}

bool relay_pcm_feed(const void *buf, unsigned size)
{
    if (relay_pcm_func.relay_pcm_feed == NULL) {
        return false;
    }
    bool ret = relay_pcm_func.relay_pcm_feed(relay_pcm_func.user_data_ptr, buf, size);
    return ret;
}
bool relay_pcm_end(void)
{
    if (relay_pcm_func.relay_pcm_end == NULL) {
        return false;
    }
    bool ret = relay_pcm_func.relay_pcm_end(relay_pcm_func.user_data_ptr);
    return ret;
}
