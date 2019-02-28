#include <cutils/log.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "set_led_api.h"

char const*const RED_LED_PATH
        = "/sys/class/gpio/gpio132/value";

char const*const GREEN_LED_PATH
        = "/sys/class/gpio/gpio129/value";

char const*const POSITION_LED_PATH
        = "/sys/class/gpio/gpio93/value";


static int
write_int(char const* path, int value)
{
    int fd;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", value);
        int amt = write(fd, buffer, bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        ALOGE("write_int failed to open %s\n", path);
        return -errno;
    }
}

int set_red_led_on(void)
{
	int ret;

	ret = write_int(RED_LED_PATH, 1);

	return ret;
}

int set_red_led_off(void)
{
	int ret;

	ret = write_int(RED_LED_PATH, 0);

	return ret;
}

int set_green_led_on(void)
{
	int ret;

	ret = write_int(GREEN_LED_PATH, 1);

	return ret;
}

int set_green_led_off(void)
{
	int ret;

	ret = write_int(GREEN_LED_PATH, 0);

	return ret;
}

int set_position_led_on(void)
{
	int ret;

	ret = write_int(POSITION_LED_PATH, 1);

	return ret;
}

int set_position_led_off(void)
{
	int ret;

	ret = write_int(POSITION_LED_PATH, 0);

	return ret;
}
