#ifndef __BIT_OPT_H__
#define __BIT_OPT_H__

#define IF_BIT_SET(val, bit)    (((val) >> (bit)) & 0x01)
#define TEST_BIT(val, bit)      ((val) & (0x01 << (bit)))
#define SET_BIT(val, bit)       ((val) |= (0x01<<(bit)))
#define SET_IN_BIT(bit)         (0x01 << (bit))
#define CLR_BIT(val, bit)       ((val) &= ~(0x01<<(bit)))
#define CLR_IN_BIT(bit)         (~(0x01 << (bit)))

#endif // __BIT_OPT_H__

