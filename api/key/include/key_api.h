#ifndef KEY_API_H
#define KEY_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
// 按键回调函数的原型
typedef void (*key_event_callback_t) (unsigned key_value, bool pressed);
// 注册回调函数
bool initKeyEvent(key_event_callback_t key_event);
// 不再关注key，不再产生回调
bool deinitKeyEvent(void);

// inner use.
void setGdiKeyEventCb(key_event_callback_t key_event_cb);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*KEY_API_H*/
