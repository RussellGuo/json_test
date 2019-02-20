#include "simple_draw.h"

#include "zh_gb_font.h"
#include "win_bitmap.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

namespace {

uint16_t FB_WIDTH = 240;
uint16_t FB_HEIGHT = 320;

color_t *fb_addr;
uint32_t line_len;

DisplayOrientation curr_display_orientation;

}


extern "C" color_t rgb565(color_t r, color_t g, color_t b)
{
    r >>= 3;
    g >>= 2;
    b >>= 3;
    return static_cast<unsigned short>((r << 11) | (g << 5) | b);

}

extern "C" void getRgbFrom565(color_t c, color_t *r, color_t *g, color_t *b)
{
    *r = static_cast<color_t>((c >> 11) << 3);
    *g = static_cast<color_t>(((c >> 5) & 63) << 2);
    *b = static_cast<color_t>((c & 31) << 3);
}

static inline color_t *getFbPtr(coord_t x, coord_t y)
{
    coord_t xx, yy;
    switch (curr_display_orientation) {
    default:
    case DMDO_0:
        xx = y;
        yy = FB_HEIGHT - 1 - x;
        break;

    case DMDO_90:
        xx = x;
        yy = y;
        break;

    case DMDO_180:
        xx = FB_WIDTH - 1 - y;
        yy = x;
        break;

    case DMDO_270:
        xx = FB_WIDTH  - 1 - x;
        yy = FB_HEIGHT - 1 - y;
        break;
    }
    if (xx >= 0 && xx < FB_WIDTH && yy >= 0 && yy < FB_HEIGHT) {
        return &fb_addr[size_t(yy) * line_len / sizeof (color_t) + size_t(xx)];
    } else {
        return nullptr;
    }
}
extern "C" void drawPoint(color_t color, coord_t x, coord_t y)
{
    color_t *p = getFbPtr(x, y);
    if (p) {
        *p = color;
    }
}

extern "C" color_t getPoint(coord_t x, coord_t y)
{
    color_t *p = getFbPtr(x, y);
    if (p) {
        return *p;
    } else {
        return 0;
    }
}

extern "C" void getScreenSize(coord_t *width, coord_t *height)
{
    switch (curr_display_orientation) {
    case DMDO_0:
    case DMDO_180:
        *width  = FB_HEIGHT;
        *height = FB_WIDTH;
        return;
    case DMDO_90:
    case DMDO_270:
        *width  = FB_WIDTH;
        *height = FB_HEIGHT;
        return;
    }
}


extern "C" void drawRect(color_t color, coord_t x0, coord_t y0, coord_t width, coord_t height)
{
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            drawPoint(color, x0 + w, y0 + h);
        }
    }
}

extern "C" void clearScreen(color_t color)
{
    coord_t width, height;
    getScreenSize(&width, &height);
    drawRect(color, 0, 0, width, height);
}

extern "C" void drawHornLine(color_t color, coord_t x0, coord_t y0, int len, int line_width)
{
    drawRect(color, x0, y0, len, line_width);
}

extern "C" void drawVertLine(color_t color, coord_t x0, coord_t y0, int len, int line_width)
{
    drawRect(color, x0, y0, line_width, len);
}


extern "C" void DisplaySetDirection(DisplayOrientation Dir)
{
    curr_display_orientation = Dir;
}

extern "C" int DisplayGetDirection(DisplayOrientation *Dir)
{
    *Dir = curr_display_orientation;
    return 0;
}

namespace {

#define FONT_PATH "/system/vendor/huaqin/fonts/"
const ChineseFont gb_font_24(FONT_PATH "gb_font_24.bin", 24);
const ChineseFont gb_font_32(FONT_PATH "gb_font_32.bin", 32);
const ChineseFont gb_font_16(FONT_PATH "gb_font_16.bin", 16);

}

inline const unsigned char *ChineseGbFontBitmap(uint16_t gb, uint16_t size)
{
    const ChineseFont *font;
    switch (size) {
    case 16:
        font = &gb_font_16;
        break;
    case 24:
        font = &gb_font_24;
        break;
    case 32:
        font = &gb_font_32;
        break;
    default:
        return nullptr;
    }
    const unsigned char *ret =font->bitmap(gb);
    return ret;
}

extern "C" void drawText(const color_t color, const color_t bg_color, int transparent, const coord_t x0, const coord_t y0, const uint8_t *const txt, const uint16_t font_size)
{
    int i = 0;
    coord_t x = x0;
    for (;;){
        uint16_t gb;
        uint16_t offset = 0;
        uint8_t chr_width = 1;
        const uint8_t *font;
        gb = txt[i++];
        if (gb == 0) {
            return;
        }
        if (gb > 127) {
            uint8_t low = txt[i++];
            if (!low) {
                return;
            }
            gb = uint16_t(gb << 8) | low;
            chr_width = 2;
        }
        font = ChineseGbFontBitmap(gb, font_size);
        if (!font) {
            continue;
        }
        for (uint8_t font_y = 0; font_y < font_size; font_y++) {
            for (uint8_t font_x = 0; font_x < font_size / 2 * chr_width; font_x++) {
                uint8_t byte = font[offset / 8];
                uint8_t v = !!(byte & 1 << ( 7 - offset % 8));
                if (v || !transparent) {
                    color_t c = v ? color : bg_color;
                    drawPoint(c, x + font_x, y0 + font_y);
                }
                offset++;
            }
        }
        x += font_size / 2 * chr_width;
    }
}

extern "C" void initDisplayMemInfo(unsigned short *_fb_addr, uint16_t width, uint16_t height, uint32_t _line_len)
{
    fb_addr = _fb_addr;
    FB_WIDTH = width;
    FB_HEIGHT = height;
    line_len = _line_len;
    fprintf(stderr, "initDisplay: %p %u %u %u\n", reinterpret_cast<void *>(fb_addr), FB_WIDTH, FB_HEIGHT, line_len);
}

extern "C" bmpHandler allocWinBmpObject(const char *bmp_file)
{
    bmpHandler h;
    h.ptr_to_win_bmp_file_object = new WinBmpFile(bmp_file);
    return h;
}
extern "C" void freeWinBmpObject(bmpHandler bmpHandler)
{
    delete reinterpret_cast<WinBmpFile *>(bmpHandler.ptr_to_win_bmp_file_object);
}

void drawMemBmp(const uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0)
{
    const uint8_t * image_data = raw_data;
    auto const _line_len = line_len ? line_len : (width * bpp + 31) / 32 * 4;
    if (bpp != 16 || image_data == nullptr) {
        fprintf(stderr, "BMP unrecognized (not 16 BPP?)\n");
        return;
    }
    for (int y = int(height -1); y >= 0; y--) {
        const color_t *line = reinterpret_cast<const color_t *>(image_data);
        for (int x = 0; x < int(width); x++) {
            drawPoint(line[x], x0 + x, y0 + y);
        }
        image_data += _line_len;
    }
}

bool saveScreenIntoMemBmp(uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0)
{
    uint8_t * image_data = raw_data;
    auto const _line_len = line_len ? line_len : (width * bpp + 31) / 32 * 4;
    if (bpp != 16 || image_data == nullptr) {
        fprintf(stderr, "BMP unrecognized (not 16 BPP?)\n");
        return false;
    }
    for (int y = int(height -1); y >= 0; y--) {
        color_t *line = reinterpret_cast<color_t *>(image_data);
        for (int x = 0; x < int(width); x++) {
            line[x] = getPoint(x0 + x, y0 + y);
        }
        image_data += _line_len;
    }
    return true;
}

void drawWinBmpObject(bmpHandler bmpHandler, coord_t x0, coord_t y0)
{
    const WinBmpFile *bmp = reinterpret_cast<WinBmpFile *>(bmpHandler.ptr_to_win_bmp_file_object);
    auto bpp        = bmp->get_bpp();
    auto width      = bmp->get_width();
    auto height     = bmp->get_height();
    auto line_len   = bmp->get_line_len();
    auto image_data = bmp->get_image_data();

    drawMemBmp(image_data, bpp, width, height, line_len, x0, y0);
}

void drawLine(color_t color, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
    int xdelta;		/* width of rectangle around line */
    int ydelta;		/* height of rectangle around line */
    int xinc;		/* increment for moving x coordinate */
    int yinc;		/* increment for moving y coordinate */
    int rem;		/* current remainder */

    /* See if the line is horizontal or vertical. If so, then call
     * special routines.
     */
    if (y1 == y2) {
        drawHornLine(color, x1, y1, x2 - x1 + 1, 1);
        return;
    }
    if (x1 == x2) {
        drawVertLine(color, x1, y1, y2 - y1 + 1, 1);
        return;
    }

    /* The line may be partially obscured. Do the draw line algorithm
     * checking each point against the clipping regions.
     */
    xdelta = x2 - x1;
    ydelta = y2 - y1;
    if (xdelta < 0)
        xdelta = -xdelta;
    if (ydelta < 0)
        ydelta = -ydelta;
    xinc = (x2 > x1)? 1 : -1;
    yinc = (y2 > y1)? 1 : -1;

    drawPoint(color, x1, y1);

    if (xdelta >= ydelta) {
        rem = xdelta / 2;
        for (;;) {
            x1 += xinc;
            rem += ydelta;
            if (rem >= xdelta) {
                rem -= xdelta;
                y1 += yinc;
            }

            drawPoint(color, x1, y1);

            if (x1 == x2)
                return;

        }
    } else {
        rem = ydelta / 2;
        for (;;) {
            y1 += yinc;
            rem += xdelta;
            if (rem >= ydelta) {
                rem -= ydelta;
                x1 += xinc;
            }

            drawPoint(color, x1, y1);

            if (y1 == y2)
                return;
        }
    }
}
