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

#include "mtts_c.h"

#include "pcm_play.h"

#include "ipc_cmd.h"

#define SAMPLE_RATE 8000

static bool tts_init(void)
{
    mtts_init();
    return true;
}
static bool tts_play(bool isGBK, const char *buf)
{
    if (isGBK) {
        send_ipc_reply("ERR INIT", 0);
        return false;
    }

    mtts_play(buf);
    send_ipc_reply("ERR OK", 0);
    return true;
}

static void tts_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    mtts_setting(pitch, rate, volume);
}

static void buzzer_play(uint16_t freq, uint16_t msec, uint16_t volume)
{
    const char *reply_msg = "ERR DEVICE";
    send_ipc_reply(reply_msg, 0);
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


int main(int argc, char *argv[])
{
    const char resource_dir[] = "/system/vendor/huaqin";
    bool ret = chdir(resource_dir);
    if (ret < 0) {
        perror("cd resource");
        exit(1);
    }

    argv0 = argv[0];
    tts_cmd_loop();
    _exit(0);
    return 0;
}
