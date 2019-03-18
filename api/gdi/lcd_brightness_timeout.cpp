#include "simple_draw.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "key_api.h"
#include "timer_api.h"

namespace {
constexpr uint16_t max_brightness = 255U;
uint16_t brightness = max_brightness;

bool enable_lcd_backlight(bool enable)
{
    int dev_fd = open("/sys/class/backlight/sprd_backlight/brightness", O_WRONLY);
    if (dev_fd < 0) {
        perror("open lcd bright device");
        return false;
    }
    char data[20];
    ssize_t len = sprintf(data, "%u", enable ? brightness : 0);
    ssize_t written = write(dev_fd, data, len);
    close(dev_fd);
    return written == len;
}

uint32_t timeout_msec;

timer_id_t gui_screen_off_timer_id = -1;

bool rerange();

void screen_off_timeout(timer_id_t, uint64_t)
{
    enable_lcd_backlight(false);
}

void key_event(unsigned, bool pressed)
{
    if (pressed) {
        rerange();
    }
}

bool rerange()
{
    static bool key_event_cb_registered = false;
    if (!key_event_cb_registered) {
        setGdiKeyEventCb(key_event);
        key_event_cb_registered = true;
    }
    if (gui_screen_off_timer_id < 0) {
        gui_screen_off_timer_id = createSimpleTimer(0, true, screen_off_timeout);
    }
    bool ret = modifySimpleTimer(gui_screen_off_timer_id, ::timeout_msec, true);
    enable_lcd_backlight(true);
    return ret;
}

template<class T>
const T& min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

}

extern "C" bool DisplaySetBrightness(uint16_t brightness)
{
    ::brightness = min(brightness, max_brightness);
    bool ret = rerange();
    return ret;
}

extern "C" bool DisplaySetScreenOffTimeout(uint32_t timeout_msec)
{
    ::timeout_msec = timeout_msec;
    bool ret = rerange();
    return ret;
}
