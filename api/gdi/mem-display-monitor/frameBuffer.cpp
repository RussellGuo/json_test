#include "frameBuffer.h"
#include <QPainter>
#include "../simple_draw.h"

#include <unistd.h>

void draw_func(void *fb, uint16_t width, uint16_t height, uint32_t linelen, volatile int *exit_flag_ptr)
{
    initDisplayMemInfo(reinterpret_cast<unsigned short *>(fb), width, height, linelen);

    color_t red    = rgb565(255,   0,   0);
    color_t green  = rgb565(  0, 255,   0);
    color_t blue   = rgb565(  0,   0, 255);
    color_t yellow = rgb565(255, 255,   0);

    clearScreen(yellow);

    for (int i = 0;; i++) {
        if (*exit_flag_ptr) {
            break;
        }
        DisplaySetDirection(DMDO_0);
        drawHornLine(red   ,  10,  20, 100, 2);
        drawVertLine(blue  ,  10,  20, 100, 2);
        drawVertLine(green , 110,  20, 100, 2);
        drawHornLine(0     ,  10, 120, 100, 2);
        DisplaySetDirection(DMDO_0);
        drawText(rgb565(192,192,192), rgb565(255,255,255), 0, 20, 150, reinterpret_cast<const uint8_t *>("\xBA\xBA\xD7\xD6\xCF\xD4\xCA\xBE""English"), 32);
        auto bmp = allocWinBmpObject("animal.bmp");
        drawWinBmpObject(bmp, 0, 0);
        freeWinBmpObject(bmp);
        uint8_t array[300 * 1024];
        uint32_t bpp = 16, width = 160, height = 120;
        auto ret = saveScreenIntoMemBmp(array, bpp, width, height, 0, 0, 0);
        if (ret) {
            drawMemBmp(array, bpp, width, height, 0, 160, 0);
            drawMemBmp(array, bpp, width, height, 0, 0, 120);
            drawMemBmp(array, bpp, width, height, 0, 160, 120);
        }
        drawLine(red, 0, 0, 120, 80);
        sleep(1);
    }
}


frameBuffer::frameBuffer() : QWidget(nullptr), exiting(false), thread(draw_func, mem, getFB_WIDTH(), getFB_HEIGHT(), getFB_WIDTH()* 2, &exiting)
{
    QSize rect(FB_HEIGHT, FB_WIDTH);
    resize(rect);
    setMinimumSize(rect);
    setMaximumSize(rect);
    startTimer(100);
}

frameBuffer::~frameBuffer()
{
    exiting = true;
    thread.join();
}

void frameBuffer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for (int fb_y = 0; fb_y < FB_HEIGHT; fb_y++) {
        for (int fb_x = 0; fb_x < FB_WIDTH; fb_x ++) {
            auto color = mem[ fb_y * FB_WIDTH + fb_x];
            auto r = (color >> 11) << 3;
            auto g = ((color >> 5) & 63) << 2;
            auto b = (color & 31) << 3;
            QPen pen(QColor(r,g,b));
            painter.setPen(pen);
            painter.drawPoint(FB_HEIGHT - 1 - fb_y, fb_x);
        }
    }
}

void frameBuffer::timerEvent(QTimerEvent *)
{
    repaint();
}
