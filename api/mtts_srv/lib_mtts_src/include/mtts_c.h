#ifndef MTTS_C_H
#define MTTS_C_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


bool mtts_init(void);
bool mtts_play(const char *buf);
void mtts_setting(uint16_t pitch, uint16_t rate, uint16_t volume);

#if defined(__cplusplus)
};
#endif


#endif
