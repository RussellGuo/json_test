#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
    config.rate = 16000;
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
    // fprintf(stderr, "PCM buffer: allocated %d bytes\r\n", size);

    do {
        num_read = fread(buffer, 1, size, file);
        if (num_read > 0) {
            if (pcm_mmap_write(pcm, buffer, num_read)) {
                fprintf(stderr, "Error playing sample\r\n");
                break;
            }
            // fprintf(stderr, "PCM read %d bytes\r\n", num_read);
        }
    } while (num_read > 0);

    free(buffer);
    pcm_close(pcm);
}

