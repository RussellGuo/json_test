
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
    fprintf(stderr, "\r%s, size: %zd\r\n", __FUNCTION__, (size_t)size);
    if (size % 2 == 1) {
        return false;
    }
    const int16_t *sample = buf;
    size_t sample_count = size / 2;
    int16_t multi_sample[sample_count * 6];
    for (size_t i = 0, j = 0; i < sample_count; i++, j+=6) {
        const int16_t v = sample[i];
        multi_sample[j] = multi_sample[j + 1] = multi_sample[j + 2] = multi_sample[j + 3] = multi_sample[j + 4] = multi_sample[j + 5] = v;
    }
    bool ret = pcm_feed(multi_sample, size * 6);
    return ret;
}

static bool file_relay_pcm_end(void *user_data_ptr)
{
    bool ret = pcm_end();
    fprintf(stderr, "\r%s\r\n", __FUNCTION__);
    return ret;
}

static void file_relay_pcm_abort(void *user_data_ptr)
{
    pcm_abort();
    fprintf(stderr, "\r%s\r\n", __FUNCTION__);
}

static struct relay_pcm_func_t relay_pcm_func = { NULL, file_replay_pcm_begin, file_relay_pcm_feed, file_relay_pcm_end, file_relay_pcm_abort};

static bool tts_should_continue(void *what)
{
    static int count;
    bool ret = ++count % 3 != 0;
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
    for (;;){
        const char *demo_tab[] = {"支付宝支付5元成功", "支付宝支付12元成功", "微信支付100元成功", "微信支付5元成功", "微信支付12元成功", "微信支付100元成功"};
        for (size_t i = 0; i < sizeof (demo_tab) / sizeof(demo_tab[0]); i++) {
            mtts_play(demo_tab[i], tts_should_continue, NULL);
            sleep(1);
        }
    }

    return 0;
}
