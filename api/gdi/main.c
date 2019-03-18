#include <stdio.h>
#include <unistd.h>

#include "simple_draw.h"
#include "poll_event_api.h"
#include "key_api.h"


static DisplayOrientation Orientation = DMDO_0;
static bmpHandler bmp;
static color_t red, green, blue, yellow;

static void draw(void)
{
    clearScreen(yellow);

    DisplaySetDirection(Orientation);
    drawHornLine(red   ,  10,  20, 100, 2);
    drawVertLine(blue  ,  10,  20, 100, 2);
    drawVertLine(green , 110,  20, 100, 2);
    drawHornLine(0     ,  10, 120, 100, 2);
    drawText(rgb565(192,192,192), rgb565(255,255,255), 0, 20, 150, (const uint8_t *)"\xBA\xBA\xD7\xD6\xCF\xD4\xCA\xBE""English", 32);
    drawWinBmpObject(bmp, 0, 0);
    displayFlush();
}

static bool should_exit = false;

static void key_event_callback (unsigned key_value, bool pressed)
{
    fprintf(stderr, "Key '%d' %s\r\n", key_value, pressed ? "pressed" : "released");
    if (!pressed) {
        return;
    }
    switch (key_value) {
    case 0 ... 9:
        Orientation++;
        if (Orientation > DMDO_270) {
            Orientation = DMDO_0;
        }
        draw();
        break;
    default:
        clearScreen(yellow);
        displayFlush();
        DisplaySetBrightness(0);
        should_exit = true;
    }
}

int main(void)
{
    initDisplay();
    bmp = allocWinBmpObject("/system/vendor/huaqin/animal.bmp");
    red    = rgb565(255,   0,   0);
    green  = rgb565(  0, 255,   0);
    blue   = rgb565(  0,   0, 255);
    yellow = rgb565(255, 255,   0);

    DisplaySetBrightness(255);
    DisplaySetScreenOffTimeout(5 * 1000);
    initKeyEvent(key_event_callback);
    draw();
    while (!should_exit) {
        PollEventSpinOnce();
    }
    freeWinBmpObject(bmp);
    deInitDisplay();
    return 0;
}

