#ifndef TTS_API_H
#define TTS_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

// TTS播放的返回结果类型。
typedef enum  {
    tts_playing_normal_finished,    // 正常结束
    tts_playing_initiation_failed,  // 初始化未完成，通常是授权失效造成
    tts_playing_user_cancelled,     // 被用户终止。通常是在未播放完成的情况下收到了新的命令
    tts_playing_device_failed,      // 设备失效，预留类型，目前没有设备失效的情况
    tts_playing_other,              // 其它，预留类型
} tts_playing_result_t;

typedef void (*tts_callback_t)(tts_playing_result_t result, const char *msg); // TTS播放的回调函数原型。结果码和原始字串是其参数

bool RemoteTtsinit(tts_callback_t);                                     // 初始化TTS，参数是回调函数。返回false情况下后续播放不可能成功
bool RemoteTtsclose(void);                                              // 关闭TTS功能。如果返回false，代表系统有问题。这个问题不应该在release版本中发生
bool RemoteTtsStopPlaying(void);                                        // 提前终止上一次播放。如果初始化失败，这个过程会失败。
bool RemoteTtsSetting(uint16_t pitch, uint16_t rate, uint16_t volume);  // 设置音高、快慢和音量。参数范围分别是50~200、500~200、0~32767 
                                                                        // 如果上面的初始化失败，这个过程会失败。
                                                                        // 100、100的音高和速率是普通值，最大/最小值是普通值的一半/一倍。音量则是0~最大音量。
bool RemoteTtsPlay(bool isGBK, const char *txt);                        // 播放声音。isGBK用于表示是GBK编码还是UTF8编码。txt是被播放的文字。
                                                                        // 如果上面的初始化失败，会直接返回，并且没有回调发生。
bool RemoteTtsBeep(uint16_t freq, uint16_t mesc);                       // 蜂鸣器发生，声音大小是上面volume设置的值。参数分别是频率Hz数和时长的毫秒数。

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TTS_API_H*/
