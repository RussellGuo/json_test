
#include "relay_pcm_play.h"
#include "pcm_play.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

static void file_replay_pcm_begin(void *user_data_ptr)
{
    pcm_begin(0);
    fprintf(stderr, "\r%s\r\n", __FUNCTION__);
}

static bool file_relay_pcm_feed(void *user_data_ptr, const void *buf, unsigned size)
{
    fprintf(stderr, "\r%s\r\n", __FUNCTION__);
    if (size % 2 == 1) {
        return false;
    }
    const int16_t *sample = buf;
    size_t sample_count = size / 2;
    for (size_t i = 0; i < sample_count; i++) {
        const int16_t v = sample[i];
        const int16_t multi_sample[] = { v, v, v, v, v, v};
        if (!pcm_feed(multi_sample, sizeof multi_sample)) {
            return false;
        }
    }
    return true;
}

static bool file_relay_pcm_end(void *user_data_ptr)
{
    bool ret = pcm_end();
    fprintf(stderr, "\r%s\r\n", __FUNCTION__);
    return ret;
}


static struct relay_pcm_func_t relay_pcm_func = { NULL, file_replay_pcm_begin, file_relay_pcm_feed, file_relay_pcm_end, };

static void feed_empty(void)
{
    static int16_t silence[ 280 * BUZZER_PCM_SAMPLE_RATE / 1000];
    pcm_feed(silence, sizeof silence);
}


#define SAMPLE_COUNT (8000 * 4)
static uint16_t sample[SAMPLE_COUNT];
int main(int argc, char *argv[])
{
#define VOLUME 32767
    for (int i = 0; i < SAMPLE_COUNT; i+=8) {
        sample[i + 0] = -VOLUME;
        sample[i + 1] = -VOLUME;
        sample[i + 2] = -VOLUME;
        sample[i + 3] = -VOLUME;
        sample[i + 4] = +VOLUME;
        sample[i + 5] = +VOLUME;
        sample[i + 6] = +VOLUME;
        sample[i + 7] = +VOLUME;
    }
    pcm_prepair();
    set_relay_pcm_callback_func(&relay_pcm_func);
    for (int k = 0; k < 3; k++) {
        replay_pcm_begin();
        feed_empty();
        relay_pcm_feed(sample , (sizeof sample) / 4);
        relay_pcm_feed(sample , (sizeof sample) / 4);
        relay_pcm_feed(sample , (sizeof sample) / 4);
        relay_pcm_feed(sample , (sizeof sample) / 4);
        feed_empty();
        relay_pcm_end();
        sleep(1);
    }
    return 0;
}
