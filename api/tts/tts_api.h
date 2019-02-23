#ifndef TTS_API_H
#define TTS_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

bool RemoteTtsinit(void);
bool RemoteTtsclose(void);
bool RemoteTtsStopPlaying(void);
bool RemoteTtsSetting(uint16_t pitch, uint16_t rate, uint16_t volume);
bool RemoteTtsPlay(bool isGBK, const char *txt);
bool RemoteTtsBeep(uint16_t freq, uint16_t mesc);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TTS_API_H*/
