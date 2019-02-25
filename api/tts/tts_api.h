#ifndef TTS_API_H
#define TTS_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

typedef enum  {
    tts_playing_normal_finished,
    tts_playing_initiation_failed,
    tts_playing_user_cancelled,
    tts_playing_device_failed,
    tts_playing_other,
} tts_playing_result_t;

typedef void (*tts_callback_t)(tts_playing_result_t result, const char *msg);

bool RemoteTtsinit(tts_callback_t);
bool RemoteTtsclose(void);
bool RemoteTtsStopPlaying(void);
bool RemoteTtsSetting(uint16_t pitch, uint16_t rate, uint16_t volume);
bool RemoteTtsPlay(bool isGBK, const char *txt);
bool RemoteTtsBeep(uint16_t freq, uint16_t mesc);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TTS_API_H*/
