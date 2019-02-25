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

#include "key_api.h"
#include "poll_event_api.h"
#include "timer_api.h"

static void key_event_callback (unsigned key_value, bool pressed)
{
    fprintf(stderr, "Key '%d' %s\r\n", key_value, pressed ? "pressed" : "released");
}

int main(void)
{
    initKeyEvent(key_event_callback);
    while (true) {
        int ret = PollEventSpinOnce();
        if (ret < 0) {
            perror("PollEventSpinOnce");
            break;
        }
    }

    deinitKeyEvent();
    return 0;
}
