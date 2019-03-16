#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#define SAMPLE_RATE 16000
#define MS_TO_SAMPLE_COUNT(msec) (msec * SAMPLE_RATE / 1000)

static bool play_sample(unsigned int card, unsigned int device, const int16_t *samples, unsigned msec)
{
    struct pcm_config config;
    struct pcm *pcm;
    bool ok = false;
    size_t sample_len_in_byte = MS_TO_SAMPLE_COUNT(msec) * 2;

    fprintf(stderr, "playing %3ums... ", msec);

    memset(&config,0x00,sizeof config);
    config.channels = 1;
    config.rate = SAMPLE_RATE;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    config.period_count = 2;
    config.period_size = msec <= 200 ? 160 : 320;

    pcm = pcm_open(card, device, PCM_OUT | PCM_MMAP | PCM_NOIRQ | PCM_MONOTONIC, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "%s\n", msec, pcm_get_error(pcm));
        return ok;
    }

    ok = pcm_mmap_write(pcm, (char *)samples, sample_len_in_byte) == 0;
    if (ok) {
        fprintf(stderr, "succeeded\n" );
    } else {
        perror("pcm_mmap_write");
    }
    pcm_close(pcm);
    return ok;
}

static void pcm_prepair(void)
{
    system("tinymix -D 0 \"SPKL Mixer DACLSPKL Switch\" 1");
    system("tinymix -D 0 \"VBC DACL DG Switch\" 1");
    system("tinymix -D 0 \"VBC DACR DG Switch\" 1");
    system("tinymix -D 0 \"SPKL Playback Volume\" 6");
    system("tinymix -D 0 \"Speaker Function\" 1");
}

#define MAX_SAMPLE_MSEC 1000
#define MAX_SAMPLE_COUNT MS_TO_SAMPLE_COUNT(MAX_SAMPLE_MSEC)

static int16_t samples[MAX_SAMPLE_COUNT];
static void prepair_samples(void)
{
#define FREQ 400
#define VOLUME 20000
    for (unsigned n = 0; n < MAX_SAMPLE_COUNT; n++) {
        samples[n] = (2 * n * FREQ / SAMPLE_RATE) % 2 == 0 ? -VOLUME : +VOLUME;
    }
}

int main(void)
{
    pcm_prepair();
    prepair_samples();
    for (;;) {
        play_sample(0, 0, samples, 80);
        sleep(2);
        play_sample(0, 0, samples, 500);
        sleep(2);
    }
    return 0;
}
