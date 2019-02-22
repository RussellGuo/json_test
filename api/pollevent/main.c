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
int main(int argc, char **argv)
{
    PollEventInit();
   return 0;
}
