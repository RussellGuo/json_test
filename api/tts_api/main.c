#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "tts_api.h"
#include "poll_event_api.h"
#include "timer_api.h"
#include "key_api.h"

static struct termios backup_tty_attr;
static int tty_fd;
static void restore_tty(void)
{
    if (tcsetattr(tty_fd, TCSANOW, &backup_tty_attr) < 0) {
        fprintf(stderr, "error in restore the attribute of tty\r\n");
    } else {
        fprintf(stderr, "restore_tty exitting\r\n");
    }
}

static void init_tty(void)
{
    struct termios tty_attr;

    tty_fd = open("/dev/tty", O_RDONLY | O_NDELAY, 0);
    if (tty_fd < 0) {
        fprintf(stderr, "Unable to open tty\r\n");
        exit(1);
    }

    memset(&tty_attr, 0, sizeof(struct termios));
    tty_attr.c_iflag = IGNPAR;
    tty_attr.c_cc[VMIN] = 1;
    tty_attr.c_cflag = B115200 | HUPCL | CS8 | CREAD | CLOCAL;

    if (tcgetattr(tty_fd, &backup_tty_attr) < 0) {
        fprintf(stderr, "Unable to get the attribute of the tty\r\n");
        exit(1);
    }

    atexit(restore_tty);
    if (tcsetattr(tty_fd, TCSANOW, &tty_attr) < 0) {
        fprintf(stderr, "Unable to set the attribute of the tty\r\n");
        exit(1);
    }
}

static void tts_callback(tts_playing_result_t result, const char *msg);

static volatile bool ctrl_c_pressed = false;
static void tty_read_proc(int fd, uint64_t arg)
{
    char c;
    ssize_t ret = read(fd, &c, 1);
    if (ret != 1) {
        ctrl_c_pressed = true;
        perror("read tty raw mode");
        fprintf(stderr, "\r\n");
    }
    fprintf(stderr, "c: '%c', arg: %llX\r\n", c, arg);

    bool tts_ret = true;
    switch(c) {
    case '1':
        tts_ret = RemoteTtsPlay(false, "To be, or not to be, that is a question");
        break;
    case '2':
        tts_ret = RemoteTtsPlay(false, "支付宝支付5元成功，支付宝支付50.5元成功，支付宝支付2000元成功，微信支付5元成功，微信支付50.5元成功，微信支付2000元成功。");
        break;
    case '3':
    case '4':
        tts_ret = RemoteTtsBeep(c == '3' ? 300 : 2700, 80);
        break;
    case '5':
    case '6':
        tts_ret = RemoteTtsBeep(c == '5' ? 300 : 2700, 500);
        break;
    case '7':
        tts_ret = RemoteTtsStopPlaying();
        break;
    case '8':
        RemoteTtsclose();
        RemoteTtsinit(tts_callback);
        break;
    case 'Q':
    case 'q':
    case 3:
        ctrl_c_pressed = true;
    }
    if (!tts_ret) {
        fprintf(stderr, "Remote TTS invoking failed\r\n");
    }
}

static void tts_callback(tts_playing_result_t result, const char *msg)
{
    fprintf(stderr, "tts_notifiy_callback: %d, '%s'\r\n", result, msg);
}

static void timeout(timer_id_t fd, uint64_t timeout_count)
{
    fprintf(stderr, "timer %d is timeout, count: %llu\r\n", fd, timeout_count);
}

static void key_event_callback (unsigned key_value, bool pressed)
{
    fprintf(stderr, "Key '%d' %s\r\n", key_value, pressed ? "pressed" : "released");
}


int main(int argc, char **argv)
{
    init_tty();
    setPollEventFd(tty_fd, tty_read_proc, 0xdeadbeef, true);
    RemoteTtsinit(tts_callback);
    // timer_id_t timer_id = createSimpleTimer(3000, false, timeout);
    initKeyEvent(key_event_callback);


    while (!ctrl_c_pressed) {
        int ret;
        fprintf(stderr, "please press key:\r\n");
        fprintf(stderr, "   1: playing English\r\n");
        fprintf(stderr, "   2: playing Utf8 Chinese\r\n");
        fprintf(stderr, "   3: playing beep in  300Hz,  80ms\r\n");
        fprintf(stderr, "   4: playing beep in 2700Hz,  80ms\r\n");
        fprintf(stderr, "   5: playing beep in  300Hz, 500ms\r\n");
        fprintf(stderr, "   6: playing beep in 2700Hz, 500ms\r\n");
        fprintf(stderr, "   7: stop last playing\r\n");
        fprintf(stderr, "   8: Restart Remote TTS\r\n");
        fprintf(stderr, "   Q: exit\r\n");
        ret = PollEventSpinOnce();
        if (ret < 0) {
            perror("PollEventSpinOnce");
            break;
        }

    }

    fprintf(stderr, "closing\r\n");
    delPollEventFd(tty_fd);
    RemoteTtsclose();
    // delTimer(timer_id);
    deinitKeyEvent();

    return 0;
}
