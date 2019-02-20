#ifndef ZH_GB_FONT_H
#define ZH_GB_FONT_H

#include <stdint.h>
#include "memory_map.h"

class ChineseFont {
public:
    ChineseFont( const char *font_path, uint16_t _font_size): map(font_path), font_size(_font_size){}
    ~ChineseFont() {}
    const unsigned char *bitmap(uint16_t gb) const {
        auto base = static_cast<const unsigned char *>(map.Memory());

        if (!base) {
            return base;
        }
        size_t offset;
        if (gb <= 127) {
            offset = A_START + gb * A_SPS;
        } else {
            uint8_t high = gb >> 8;
            uint8_t low = gb &0xFF;
            offset = C_START + ((high - 0x81)*191 + (low - 0x40))*C_SPS;
        }
        if (offset + C_SPS > map.Size()) {
            return nullptr;
        }
        return base + offset;
    }

private:
    const TMemoryMapFile map;
    const uint16_t font_size;
    const uint16_t A_SUM = 128;
    const uint16_t A_RES = uint16_t(font_size * font_size / 2);
    const uint16_t A_SPS = A_RES / 8;
    const uint16_t A_START = 0;
    const uint16_t C_START = A_START + A_SUM * A_SPS;
    const uint16_t C_RES = uint16_t(font_size * font_size);
    const uint16_t C_SPS = C_RES / 8;
};


#endif // ZH_GB_FONT_H
