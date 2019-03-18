#ifndef SIMPLE_DRAW_H
#define SIMPLE_DRAW_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

// 颜色类型，只支持RGB565
typedef uint16_t color_t;
// 坐标类型，x/y都是整数
typedef int coord_t;

// 显示方向定义
typedef enum {
    DMDO_0, DMDO_90, DMDO_180, DMDO_270,
} DisplayOrientation;
// 设置和获取显示方向
void DisplaySetDirection(DisplayOrientation Dir);
int DisplayGetDirection(DisplayOrientation *Dir);
// 屏幕横纵像素数。注意方向旋转时候，值是不一样的
void getScreenSize(coord_t *width, coord_t *height);

// 关于rgb565颜色的合成和分解。r/g/b都是 0~255的辉度范围
color_t rgb565(color_t r, color_t g, color_t b);
void getRgbFrom565(color_t c, color_t *r, color_t *g, color_t *b);

// 绘制指定点以指定颜色，用RGB565组合值表达
void drawPoint(color_t color, coord_t x, coord_t y);
//获得指定点的颜色。屏幕外一概是0
color_t getPoint(coord_t x, coord_t y); // return color of x,y; 0 if out of the screen

//填充矩形绘制。颜色、起始坐标和横纵值
void drawRect(color_t color, coord_t x0, coord_t y0, coord_t width, coord_t height);
// 清屏。其实就是全举行填充。
void clearScreen(color_t color);

// 横线绘制和纵线绘制。颜色、起点、长度、线宽在参数中
void drawHornLine(color_t color, coord_t x0, coord_t y0, int len, int line_width);
void drawVertLine(color_t color, coord_t x0, coord_t y0, int len, int line_width);
// 斜线绘制。起始点在参数中。线宽是1. 斜线的线宽是很麻烦的，跟倾斜度有关。
void drawLine(color_t color, coord_t x1, coord_t y1, coord_t x2, coord_t y2);

// 文本绘制，字体是客户提供的GBK编码
// color是文字的颜色，bg_color是背景颜色（如果transparent非0，则忽略——
// x0/y0是起始位置，txt是GBK编码的文字，font_size是24/32
// if transparent != 0, then will keep background else draw a bk_color for text's blank.
void drawText(const color_t color, const color_t bg_color, int transparent, const coord_t x0, const coord_t y0, const uint8_t *const txt, const uint16_t font_size);

// 内部使用，除非已经十分了解其用法
void initDisplayMemInfo(unsigned short *_fb_addr, uint16_t width, uint16_t height, uint32_t _line_len);

// 初始化/去初始化
void initDisplay(void);
void deInitDisplay(void);
// 绘制完成后，发往屏幕
void displayFlush(void);

// Windows BMP绘制相关
// 指针类型被“强类型化”
typedef struct {
    void *ptr_to_win_bmp_file_object;
} bmpHandler;

// 给定BMP文件，获得BMP对象.目前只支持RGB565。其它格式的随后访问都什么都不发生。
bmpHandler allocWinBmpObject(const char *bmp_file);
// 释放对象
void freeWinBmpObject(bmpHandler bmpHandler);
// 绘制此BMP到指定位置
void drawWinBmpObject(bmpHandler bmpHandler, coord_t x0, coord_t y0);

// 保存屏幕到内存。目前只支持RGB565
// line_len一般放0，系统会根据width计算line_len，成为4字节对齐的字节数。这个参数是用于在一块大的raw_data上获取部分屏幕数据的。绝大部分场合放0
bool saveScreenIntoMemBmp(uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0);
// 保存过的屏幕数据，恢复到屏幕的指定位置
void drawMemBmp(const uint8_t *const raw_data, uint32_t bpp, uint32_t width, uint32_t height, uint32_t line_len, coord_t x0, coord_t y0);

// 设置屏幕亮度,0~255。0则灭屏, 最高亮度为255，大于255按照255算。
bool DisplaySetBrightness(uint16_t brightness);
// 设置自动灭屏超时时间，任何按键动作都会导致重新计时。timeout_msec: 超时时间，单位为毫秒，0则不自动灭屏
//
bool DisplaySetScreenOffTimeout(uint32_t timeout_msec);

#if defined(__cplusplus)
};
#endif

#endif // SIMPLE_DRAW_H
