#ifndef WIN_BITMAP_H
#define WIN_BITMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef  struct __attribute__ ((packed)) tagBITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct __attribute__ ((packed)) tagBITMAPINFOHEADER {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

#include "memory_map.h"

class WinBmpFile {
public:
    explicit WinBmpFile(const char *bmp_name): map(bmp_name), image_data(nullptr), width(0), height(0), line_len(0), bpp(0) {
        const uint8_t *base = reinterpret_cast<const uint8_t *>(map.Memory());
        if (base == nullptr) {
            return;
        }

        const BITMAPFILEHEADER *bmp_file_header = reinterpret_cast<const BITMAPFILEHEADER *>(base);
        const BITMAPINFOHEADER *bmp_info_header = reinterpret_cast<const BITMAPINFOHEADER *>(base + sizeof(BITMAPFILEHEADER));

        if (bmp_file_header->bfType != 0x4D42) { // BM
            return;
        }
        fprintf(stderr, "%x\n", bmp_file_header->bfOffBits);
        if (bmp_info_header->biWidth <= 0 || bmp_info_header->biHeight <= 0) {
            return;
        }
        auto const _bpp    = bmp_info_header->biBitCount;
        auto const _width  = uint32_t(bmp_info_header->biWidth);
        auto const _height = uint32_t(bmp_info_header->biHeight);
        auto const _line_len = (_width * _bpp / 8 + 3) / 4 * 4; // align to 4
        auto const tail = bmp_file_header->bfOffBits + _line_len * _height;
        if (tail > map.Size()) {
            return;
        }
        bpp      = _bpp;
        width    = _width;
        height   = _height;
        line_len = _line_len;
        image_data = base + bmp_file_header->bfOffBits;
    }

    static void* operator new(size_t sz) {
        auto ret = malloc(sz > 0 ? sz : 4);
        return ret;
    }
    void operator delete(void *p) {
        free(p);
    }

    static void* operator new[](size_t sz) = delete;
    void operator delete[](void* p) = delete;
    const uint8_t *get_image_data() const {
        return image_data;
    }

    uint32_t get_width() const {
        return width;
    }

    uint32_t get_height() const {
        return height;
    }

    uint32_t get_line_len() const {
        return line_len;
    }

    uint32_t get_bpp() const {
        return bpp;
    }

private:
    const TMemoryMapFile map;
    const uint8_t *image_data;
    uint32_t width, height;
    uint32_t line_len;
    uint32_t bpp;
};

#endif // WIN_BITMAP_H








