#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "mtts_c.h"

#include "relay_pcm_play.h"

#include "pcm_play.h"

#include "ipc_cmd.h"

// TODO: reduce memory cost of the buzzer playing
#define BUZZER_MIN_FRQ 100
#define BUZZER_MAX_FRQ (BUZZER_PCM_SAMPLE_RATE / 2)
#define BUZZER_MIN_MSEC 70
#define BUZZER_MAX_MSEC 600

#define MAX_SAMPLE_COUNT (BUZZER_MAX_MSEC * BUZZER_PCM_SAMPLE_RATE / 1000)

static void feed_empty(void)
{
    static int16_t silence[ 30 * BUZZER_PCM_SAMPLE_RATE / 1000];
    pcm_feed(silence, sizeof silence);
}

static void buzzer_play(uint16_t freq, uint16_t msec, uint16_t volume)
{
    bool feed_ok = true;
    bool cancelled = false;
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
    size_t sample_count = msec * BUZZER_PCM_SAMPLE_RATE / 1000;
    volume = volume * 7 / 10;
    for (unsigned n = 0; n < sample_count; n++) {
        sample[n] = (2 * n * freq / BUZZER_PCM_SAMPLE_RATE) % 2 == 0 ? -volume : +volume;
        if (n == 0 || sample[n] != sample[n - 1]) {
            // fprintf(stderr, "sample[%u] = %d\r\n", n, sample[n]);
        }
    }

    pcm_begin(0);
    feed_empty();
    feed_ok = pcm_feed(sample, sample_count * 2);
    feed_empty();
    if (cancelled) {
        pcm_abort();
    }
    feed_ok = (!!feed_ok) & (!!pcm_end());
    const char *reply_msg;
    if (!feed_ok) {
        reply_msg = "ERR DEVICE";
    } else {
        reply_msg = cancelled ? "ERR USERCANCELLED" : "ERR OK";
    }
    send_ipc_reply(reply_msg, 0);
}

static bool tts_should_continue(void *what)
{
    bool ret = !has_ipc_cmd_from_caller(0);
    return ret;
}

static bool tts_init(void)
{
    bool ret = mtts_init();
    return ret;
}
static bool tts_play(bool isGBK, const char *buf)
{
    if (isGBK) {
        send_ipc_reply("ERR INIT", 0);
        return false;
    }

    mtts_play(buf, tts_should_continue, NULL);
    pcm_end();
    send_ipc_reply("ERR OK", 0);
    return true;
}

static void tts_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    mtts_setting(pitch, rate, volume);
}

static void tts_cmd_loop(void)
{

    uint16_t pitch = 105, rate = 105, volume = 32767;
    pid_t parent_pid = getppid();

    bool ret = tts_init();
    fprintf(stderr, "return value of tts init: %d\r\n", ret);
    if (!ret) {
        // return; // should NOT quit
    }
    tts_setting(pitch, rate, volume);

    for (;;) {
            char buf[4096];
            char cmd;
            memset(buf, 0, sizeof buf);
            int len = recv_ipc_cmd(buf, sizeof buf);
            if (len < 0) {
                perror("recv_msg_from maintask");
                fprintf(stderr, "\r\n");
                continue;
            }
            cmd = toupper(buf[0]);
            switch (cmd) {
            case 'Q':
                fprintf(stderr, "Quit Cmd received\r\n");
                return;

            case 'B':

                {
                    uint16_t freq = 0, msec = 0;
                    fprintf(stderr, "Beep Cmd received\r\n");
                    if (sscanf(buf + 1, "%hu%hu", &freq, &msec) == 2) {
                        fprintf(stderr, "Beepping in %u %u\r\n", freq, msec);
                        buzzer_play(freq, msec, volume);
                    } else {
                        fprintf(stderr, "Beepping argv error: %s\r\n", buf + 1);
                    }
                }
                break;

            case 'S':
                {
                    fprintf(stderr, "Setting Cmd received\r\n");
                    if (sscanf(buf + 1, "%hu%hu%hu", &pitch, &rate, &volume) == 3) {
                        fprintf(stderr, "Setting in %u %u %u\r\n", pitch, rate, volume);
                        tts_setting(pitch, rate, volume);
                    } else {
                        fprintf(stderr, "Setting argv error: %s\r\n", buf + 1);
                    }
                }
                break;

            case 'G':
            case 'U':
                {
                    bool isGBK = buf[0] == 'G';
                    fprintf(stderr, "ttf Playing Cmd received\r\n");
                    fprintf(stderr, "tts playing argv: %s %s\r\n", isGBK ? "GBK": "UTF8", buf + 1);
                    tts_play(isGBK, buf + 1);
                }
                break;

            case 'T':
                {
                fprintf(stderr, "STOP cmd received\r\n");
                }
                break;

            default:
                fprintf(stderr, "Unknown cmd: %s\r\n", buf);
                break;
            }
    }
}

static char *argv0;
void set_cmd_line_argv0(const char *cmd_line_argv0)
{
    if (argv0 == NULL || strlen(cmd_line_argv0) > strlen(argv0)) {
        return;
    }
    strcpy(argv0, cmd_line_argv0);
}

static bool is_stdin_a_socket(void)
{
    struct stat sb;
    fstat(0, &sb);
    if ((sb.st_mode & S_IFMT) == S_IFSOCK) {
        return true;
    }
    return false;
}

static void file_replay_pcm_begin(void *user_data_ptr)
{
    pcm_begin(0);
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
    return ret;
}


static struct relay_pcm_func_t relay_pcm_func = { NULL, file_replay_pcm_begin, file_relay_pcm_feed, file_relay_pcm_end, };

int main(int argc, char *argv[])
{
    const char resource_dir[] = "/system/vendor/huaqin";
    bool ret = chdir(resource_dir);
    if (ret < 0) {
        perror("cd resource");
        exit(1);
    }

    argv0 = argv[0];
    pcm_prepair();
    set_relay_pcm_callback_func(&relay_pcm_func);

    if (is_stdin_a_socket()) {
        tts_cmd_loop();
        _exit(0);
    } else {
        tts_init();
        tts_play(false, argv[1]?:"我是马首我是马首");
    }
    return 0;
}
