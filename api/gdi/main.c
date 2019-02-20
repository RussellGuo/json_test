#include <stdio.h>
#include <unistd.h>

#include "simple_draw.h"


int main(void)
{
    initDisplay();

    color_t red    = rgb565(255,   0,   0);
    color_t green  = rgb565(  0, 255,   0);
    color_t blue   = rgb565(  0,   0, 255);
    color_t yellow = rgb565(255, 255,   0);

    bmpHandler bmp = allocWinBmpObject("/system/vendor/huaqin/animal.bmp");

    for (int i = 0; i < 200; i++) {
        clearScreen(yellow);

        DisplaySetDirection(i % 4);
        drawHornLine(red   ,  10,  20, 100, 2);
        drawVertLine(blue  ,  10,  20, 100, 2);
        drawVertLine(green , 110,  20, 100, 2);
        drawHornLine(0     ,  10, 120, 100, 2);
        drawText(rgb565(192,192,192), rgb565(255,255,255), 0, 20, 150, (const uint8_t *)"\xBA\xBA\xD7\xD6\xCF\xD4\xCA\xBE""English", 32);
        drawWinBmpObject(bmp, 0, 0);
        displayFlush();

        usleep(100 * 1000);
    }
    freeWinBmpObject(bmp);


    deInitDisplay();
    return 0;
}

