#ifndef MTTS_C_H
#define MTTS_C_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


bool tts_init(void);
bool tts_play(bool isGBK, const char *buf);
void tty_setting(uint16_t pitch, uint16_t rate, uint16_t volume);

#if defined(__cplusplus)
};
#endif


#endif
