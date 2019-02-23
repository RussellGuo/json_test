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

static volatile bool ctrl_c_pressed = false;
static void tty_read_proc(int fd, uint64_t arg)
{
    char c;
    ssize_t ret = read(fd, &c, 1);
    fprintf(stderr, "ret, c: %zd, %c, arg: %lx\r\n", ret, c, arg);
    if (ret == 1 && c == 3) {
        ctrl_c_pressed = true;
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
    setPollEventFd(tty_fd, tty_read_proc, 0x19710829U, true);

    while (!ctrl_c_pressed) {
        int ret = PollEventSpinOnce();
        if (ret < 0) {
            perror("PollEventSpinOnce");
            break;
        }
    }
    fprintf(stderr, "closing\r\n");
    return 0;
}
