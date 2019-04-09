#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>

#include <unistd.h>
#include <sys/wait.h>

#include "pcm_play.h"

// remote PCM playing part
void pcm_prepair(void)
{
    system("tinymix -D 0 \"SPKL Mixer DACLSPKL Switch\" 1");
    system("tinymix -D 0 \"VBC DACL DG Switch\" 1");
    system("tinymix -D 0 \"VBC DACR DG Switch\" 1");
    system("tinymix -D 0 \"SPKL Playback Volume\" 6");
    system("tinymix -D 0 \"Speaker Function\" 1");
}

static int pcm_pipe_fds[2] = { -1, -1 };
static pid_t pid;

void pcm_begin(void)
{
    // pipe
    if (pcm_pipe_fds[0] >= 0 || pcm_pipe_fds[1] >= 0) {
        fprintf(stderr, "Calling pcm_begin without closing it\r\n");
        return;
    }
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pcm_pipe_fds) < 0) {
        perror("pcm pipe");
        fprintf(stderr, "\r\n");
        return;
    }
    // fork
    pid = fork();
    if (pid < 0) {
        perror("pcm fork");
        fprintf(stderr, "\r\n");
        pcm_end();
        return;
    } else if(pid > 0) {
        // main process
        // main should not use pipe reading
        close(pcm_pipe_fds[0]);
        pcm_pipe_fds[0] = -1;
        return;

    } else {
        // sub process
        // void set_cmd_line_argv0(const char *cmd_line_argv0);
        // set_cmd_line_argv0("[PCM]");
        FILE *read_file = fdopen(pcm_pipe_fds[0], "rb");

        // sub process should not use pipe writting
        close(pcm_pipe_fds[1]);
        pcm_pipe_fds[1] = -1;

        if (read_file == NULL) {
            fprintf(stderr, "fdopen error\r\n");
            exit(1);
        } else {
            void play_sample(FILE *file, unsigned int card, unsigned int device);
            play_sample(read_file, 0, 0);
            fprintf(stderr, "pcm sub process closing...\r\n");
            fclose(read_file);
            _exit(0);
        }
    }
}

void pcm_feed(const void *buf, unsigned size)
{

    ssize_t len;
    // write pipe
    if (pcm_pipe_fds[1] < 0) {
        fprintf(stderr, "Calling feed_pcm without beginning it\r\n");
        return;
    }
    len = write(pcm_pipe_fds[1], buf, size);
    if (len != (ssize_t)size) {
        fprintf(stderr, "pcm pipe write error ret = %d\r\n", len);
    }
}

void pcm_abort(void)
{
    fprintf(stderr, "pcm main process aborting...\r\n");
    if (pid >= 0) {
        kill(pid, SIGQUIT);
    }
}

void pcm_end(void)
{
    fprintf(stderr, "pcm main process ending...\r\n");
    // close pipe
    for (int i = 0; i < 2; i++) {
        if (pcm_pipe_fds[i] >= 0) {
            close(pcm_pipe_fds[i]);
            pcm_pipe_fds[i] = -1;
        }
    }
    // wait sub-process
    if (pid >= 0) {
        int status;
        waitpid(pid, &status, 0);
        pid = -1;
    }
}

void play_sample(FILE *file, unsigned int card, unsigned int device)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    int size;
    int num_read;


    memset(&config,0x00,sizeof(struct pcm_config));
    config.channels = 1;
    config.rate = 8000;
    config.period_size = 1024;
    config.period_count = 4;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open(card, device, PCM_OUT | PCM_MMAP | PCM_NOIRQ | PCM_MONOTONIC, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\r\n", device, pcm_get_error(pcm));
        return;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\r\n", size);
        free(buffer);
        pcm_close(pcm);
        return;
    }
    fprintf(stderr, "PCM buffer: allocated %d bytes\r\n", size);

    do {
        num_read = fread(buffer, 1, size, file);
        if (num_read > 0) {
            if (pcm_mmap_write(pcm, buffer, num_read)) {
                fprintf(stderr, "Error playing sample\r\n");
                break;
            }
             fprintf(stderr, "PCM read %d bytes\r\n", num_read);
        }
    } while (num_read > 0);

    free(buffer);
    pcm_close(pcm);
}

#define SAMPLE_RATE 16000

#define BUZZER_MIN_FRQ 2300
#define BUZZER_MAX_FRQ 3000
#define BUZZER_MIN_MSEC 70
#define BUZZER_MAX_MSEC 600

#define MAX_SAMPLE_COUNT (BUZZER_MAX_MSEC * SAMPLE_RATE / 1000)
bool pcm_local_buzzer_play(uint16_t _freq, uint16_t _msec, uint16_t _volume)
{
    uint16_t freq = _freq, msec = _msec, volume = _volume * 7 / 10;
    if (freq < BUZZER_MIN_FRQ) {
        freq = BUZZER_MIN_FRQ;
    } else if (freq > BUZZER_MAX_FRQ) {
        freq = BUZZER_MAX_FRQ;
    }
    if (msec < BUZZER_MIN_MSEC) {
        msec = BUZZER_MIN_MSEC;
    } else if (msec > BUZZER_MAX_MSEC) {
        msec = BUZZER_MAX_MSEC;
    }
    int16_t sample[MAX_SAMPLE_COUNT];
    size_t sample_count = msec * SAMPLE_RATE / 1000;
    for (unsigned n = 0; n < sample_count; n++) {
        sample[n] = (2 * n * freq / SAMPLE_RATE) % 2 == 0 ? -volume : +volume;
        if (n == 0 || sample[n] != sample[n - 1]) {
            // fprintf(stderr, "sample[%u] = %d\r\n", n, sample[n]);
        }
    }
}

