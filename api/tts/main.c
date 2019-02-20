#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include "tts_api.h"

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

    RemoteTtsinit();

    for (;;) {
        char c;
        struct pollfd poll_fd_tab[2];
        int ret;

        fprintf(stderr, "please press key:\r\n");
        fprintf(stderr, "   1: playing English\r\n");
        fprintf(stderr, "   2: playing Utf8 Chinese\r\n");
        fprintf(stderr, "   3: playing beep in 300Hz, 10s\r\n");
        fprintf(stderr, "   4: playing beep in 1000Hz, 10s\r\n");
        fprintf(stderr, "   5: stop last playing\r\n");
        fprintf(stderr, "   Q: exit\r\n");

        poll_fd_tab[0].fd = tty_fd;
        poll_fd_tab[0].events = POLL_IN;
        poll_fd_tab[0].revents = 0;
        poll_fd_tab[1].fd = getRemoteTtsIpcFd();
        poll_fd_tab[1].events = POLL_IN;
        poll_fd_tab[1].revents = 0;
        ret = poll(poll_fd_tab, 2, 0xFFFFFFFFU);
        fprintf(stderr, "poll ret, error msg: %d, '%s'\r\n", ret, strerror(errno));

        if (poll_fd_tab[0].revents) {
            //tty has data
            ret = read(tty_fd, &c, 1);
            fprintf(stderr, "ret, c: %d, %c\r\n", ret, c);
            if (ret <= 0 || c == 'q') {
                break;
            }
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
            }
        }

        if (poll_fd_tab[1].revents) {
            char buf[4096];
            memset(buf, 0, sizeof buf);
            int len = RemoteTtsMsgFromService( buf, sizeof buf);
            fprintf(stderr, "msg from TTS ipc msg and len: %s, %d\r\n", buf, len);
        }
    }

    RemoteTtsclose();

    return 0;
}
