#ifndef KEY_API_H
#define KEY_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

typedef void (*key_event_callback_t) (unsigned key_value, bool pressed);
bool initKeyEvent(key_event_callback_t key_event);
bool deinitKeyEvent(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*KEY_API_H*/
