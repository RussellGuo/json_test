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

#include <linux/input.h>

#include "poll_event_api.h"

static struct termios backup_tty_attr;
static int tty_fd;
static void restore_tty(void)
{
    if (tcsetattr(tty_fd, TCSANOW, &backup_tty_attr) < 0) {
        fprintf(stderr, "error in restore the attribute of tty\r\n");
    } else {
        fprintf(stderr, "closed\r\n");
    }
}

int main(void)
{
    struct termios tty_attr;

    tty_fd = open("/dev/tty", O_RDONLY|O_NDELAY, 0);
    if (tty_fd < 0) {
        fprintf(stderr, "Unable to open tty\r\n");
        exit(1);
    }

    memset(&tty_attr, 0, sizeof(struct termios));
    tty_attr.c_iflag = IGNPAR;
    tty_attr.c_cc[VMIN] = 1;
    tty_attr.c_cflag = B115200|HUPCL|CS8|CREAD|CLOCAL;

    if (tcgetattr(tty_fd, &backup_tty_attr) < 0) {
        fprintf(stderr, "Unable to get the attribute of the tty\r\n");
        exit(1);
    }

    atexit(restore_tty);
    if (tcsetattr(tty_fd, TCSANOW, &tty_attr) < 0) {
        fprintf(stderr, "Unable to set the attribute of the tty\r\n");
        exit(1);
    }

    PollEventInit();
    PollEventSetFd(event_idx_key, tty_fd);
    bool event_fd_set[event_idx_max];
    memset(event_fd_set, 0, sizeof event_fd_set);
    event_fd_set[event_idx_key] = true;

    for (;;) {
        bool ret = PollEventDectect(event_fd_set, event_idx_key + 1);
        if (ret) {
            char c;
            ret = read(tty_fd, &c, 1);
            fprintf(stderr, "ret, c: %d, %c\r\n", ret, c);
            if (ret == 1 && c == 3) {
                break;
            }
        }
    }
    fprintf(stderr, "closing\r\n");
    return 0;
}
