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

void pcm_begin(unsigned msec)
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
        void set_cmd_line_argv0(const char *cmd_line_argv0);
        set_cmd_line_argv0("[PCM]");
        FILE *read_file = fdopen(pcm_pipe_fds[0], "rb");

        // sub process should not use pipe writting
        close(pcm_pipe_fds[1]);
        pcm_pipe_fds[1] = -1;

        if (read_file == NULL) {
            fprintf(stderr, "fdopen error\r\n");
            exit(1);
        } else {
            bool play_sample(FILE *file, unsigned int card, unsigned int device, unsigned msec);
            bool ret = play_sample(read_file, 0, 0, msec);
            fprintf(stderr, "pcm sub process closing...\r\n");
            fclose(read_file);
            _exit(!ret);
        }
    }
}

bool pcm_feed(const void *buf, unsigned size)
{

    ssize_t len;
    // write pipe
    if (pcm_pipe_fds[1] < 0) {
        fprintf(stderr, "Calling feed_pcm without beginning it\r\n");
        return false;
    }
    len = write(pcm_pipe_fds[1], buf, size);
    if (len != (ssize_t)size) {
        perror("pcm pipe write");
        fprintf(stderr, "\r\n");
        return false;
    }
    return true;
}

void pcm_abort(void)
{
    fprintf(stderr, "pcm main process aborting...\r\n");
    if (pid >= 0) {
        kill(pid, SIGQUIT);
    }
}

bool pcm_end(void)
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
        int _pid = waitpid(pid, &status, 0);
        pid = -1;
        return _pid != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
    } else {
        return false;
    }
}

bool play_sample(FILE *file, unsigned int card, unsigned int device, unsigned msec)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    int size;
    int num_read;


    memset(&config,0x00,sizeof(struct pcm_config));
    config.channels = 1;
    config.rate = BUZZER_PCM_SAMPLE_RATE;
    config.period_size = 1024;
    config.period_count = 4;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open(card, device, PCM_OUT | PCM_MMAP | PCM_NOIRQ | PCM_MONOTONIC, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\r\n", device, pcm_get_error(pcm));
        return false;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\r\n", size);
        free(buffer);
        pcm_close(pcm);
        return false;
    }
    // fprintf(stderr, "PCM buffer: allocated %d bytes\r\n", size);

    bool ok = true;
    do {
        num_read = fread(buffer, 1, size, file);
        if (num_read > 0) {
            if (pcm_mmap_write(pcm, buffer, num_read)) {
                perror("playing sample");
                fprintf(stderr, "\r\n");
                ok = false;
                break;
            }
            // fprintf(stderr, "PCM read %d bytes\r\n", num_read);
        }
    } while (num_read > 0);

    free(buffer);
    pcm_close(pcm);
    return ok;
}

