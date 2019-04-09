#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sched.h>

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

    config.period_count = 4;
    config.period_size = 1024;

    pcm = pcm_open(card, device, PCM_OUT | PCM_MMAP | PCM_NOIRQ | PCM_MONOTONIC, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "%u, %s\n", msec, pcm_get_error(pcm));
        return ok;
    }

    for (;;) {
        //sleep(1);
        ok = pcm_mmap_write(pcm, (char *)samples, sample_len_in_byte) == 0;
        if (ok) {
            fprintf(stderr, "succeeded\n" );
        } else {
            perror("pcm_mmap_write");
        }
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
static void prepair_samples(uint16_t volume)
{
#define FREQ 400
    for (unsigned n = 0; n < MAX_SAMPLE_COUNT; n++) {
        samples[n] = (2 * n * FREQ / SAMPLE_RATE) % 2 == 0 ? -volume : +volume;
    }
}

static void set_highest_priority(void)
{

      struct sched_param param;
      int maxpri;
      maxpri = sched_get_priority_max(SCHED_FIFO);
      if(maxpri == -1)
      {
            perror("sched_get_priority_max() failed");
            exit(1);
      }
      fprintf(stderr, "maxpri %d\n", maxpri);
      param.sched_priority = maxpri;
      if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1)

     {
            perror("sched_setscheduler() failed");
            exit(1);
     }
}

int main(int c, char *argv[])
{
    uint16_t vol;
    vol = atoi( argv[1] ?: "32767");
    set_highest_priority();
    pcm_prepair();
    prepair_samples(vol);
    play_sample(0, 0, samples, 500);
    return 0;
}
