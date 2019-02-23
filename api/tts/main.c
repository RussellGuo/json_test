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

#include <linux/input.h>

static struct termios backup_tty_attr;
static int tty_fd;
static void restore_tty(void)
{
    if (tcsetattr(tty_fd, TCSANOW, &backup_tty_attr) < 0) {
        fprintf(stderr, "error in restore the attribute of tty\r\n");
    } else {
        fprintf(stderr, "exitting\r\n");
    }
}

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
    fprintf(stderr, "ret, c: %zd, %c, arg: %lx\r\n", ret, c, arg);
    switch(c) {
    case '1':
        RemoteTtsPlay(false, "To be, or not to be, that is a question: Whether it's nobler in the mind to suffer, "
                             "The slings and arrows of outrageous fortune, Or to take arms against a sea of troubles");
        break;
    case '2':
        RemoteTtsPlay(false, "生存还是毁灭，这是一个值得考虑的问题；默然忍受命运暴虐的毒箭，或是挺身反抗人世无涯的苦难,通过斗争把它们扫个干净？");
        break;
    case '3':
    case '4':
        RemoteTtsBeep(c == '3' ? 300 : 1000, 10000);
        break;
    case '5':
        RemoteTtsStopPlaying();
        break;
    case 'Q':
    case 'q':
    case 3:
        ctrl_c_pressed = true;
    }
}


int main(int argc, char **argv)
{
    struct termios tty_attr;

    tty_fd = open("/dev/tty", O_RDWR | O_NDELAY, 0);
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
    setPollEventFd(tty_fd, tty_read_proc, 0x19710829U, true);

    RemoteTtsinit();

    while (!ctrl_c_pressed) {
        int ret;
        fprintf(stderr, "please press key:\r\n");
        fprintf(stderr, "   1: playing English\r\n");
        fprintf(stderr, "   2: playing Utf8 Chinese\r\n");
        fprintf(stderr, "   3: playing beep in 300Hz, 10s\r\n");
        fprintf(stderr, "   4: playing beep in 1000Hz, 10s\r\n");
        fprintf(stderr, "   5: stop last playing\r\n");
        fprintf(stderr, "   Q: exit\r\n");
        ret = PollEventSpinOnce();
        if (ret < 0) {
            perror("PollEventSpinOnce");
            break;
        }

    }

    fprintf(stderr, "closing\r\n");
    RemoteTtsclose();

    return 0;
}
