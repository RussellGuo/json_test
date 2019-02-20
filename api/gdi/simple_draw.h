#ifndef SIMPLE_DRAW_H
#define SIMPLE_DRAW_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint16_t color_t;
typedef int coord_t;

typedef enum {
    DMDO_0, DMDO_90, DMDO_180, DMDO_270,
} DisplayOrientation;
void DisplaySetDirection(DisplayOrientation Dir);
int DisplayGetDirection(DisplayOrientation *Dir);
void getScreenSize(coord_t *width, coord_t *height);

color_t rgb565(color_t r, color_t g, color_t b);
void getRgbFrom565(color_t c, color_t *r, color_t *g, color_t *b);

void drawPoint(color_t color, coord_t x, coord_t y);
color_t getPoint(coord_t x, coord_t y); // return color of x,y; 0 if out of the screen

void drawRect(color_t color, coord_t x0, coord_t y0, coord_t width, coord_t height);
void clearScreen(color_t color);

void drawHornLine(color_t color, coord_t x0, coord_t y0, int len, int line_width);
void drawVertLine(color_t color, coord_t x0, coord_t y0, int len, int line_width);
void drawLine(color_t color, coord_t x1, coord_t y1, coord_t x2, coord_t y2);

// if transparent != 0, then will keep background else draw a bk_color for text's blank.
void drawText(const color_t color, const color_t bg_color, int transparent, const coord_t x0, const coord_t y0, const uint8_t *const txt, const uint16_t font_size);

void initDisplayMemInfo(unsigned short *_fb_addr, uint16_t width, uint16_t height, uint32_t _line_len);

void initDisplay(void);
void deInitDisplay(void);
void displayFlush(void);

typedef struct {
    void *ptr_to_win_bmp_file_object;
} bmpHandler;

bmpHandler allocWinBmpObject(const char *bmp_file);
void freeWinBmpObject(bmpHandler bmpHandler);
void drawWinBmpObject(bmpHandler bmpHandler, coord_t x0, coord_t y0);

void drawMemBmp(const uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0);
bool saveScreenIntoMemBmp(uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0);

#if defined(__cplusplus)
};
#endif

#endif // SIMPLE_DRAW_H
