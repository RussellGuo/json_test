#ifndef ADF_CLASS_H
#define ADF_CLASS_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

bool initAdfDevice(void **fb_ptr, uint32_t *width, uint32_t *height, uint32_t *linelen);
void deinitAdfDevice(void);
void AdfFlip(void);

#if defined(__cplusplus)
}
#endif

#endif // ADF_CLASS_H
