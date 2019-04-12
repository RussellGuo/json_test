
#include "mtts_c.h"
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

static bool tts_should_continue(void *what)
{
    bool ret = true;
    return ret;
}

int main(int argc, char *argv[])
{
    pcm_prepair();
    const char resource_dir[] = "/system/vendor/huaqin";
    int ret = chdir(resource_dir);
    if (ret < 0) {
        perror("cd resource");
        exit(1);
    }

    set_relay_pcm_callback_func(&relay_pcm_func);

    mtts_init();
    mtts_play(argv[1]?:"我是马首我是马首", tts_should_continue, NULL);
    mtts_play("智能驾驶背后极简设计", tts_should_continue, NULL);

    return 0;
}
