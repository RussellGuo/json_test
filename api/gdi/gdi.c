#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "adf_class.h"
#include "simple_draw.h"

void initDisplay(void)
{
    void *fb_ptr;
    uint32_t width, height, linelen;
    bool ret = initAdfDevice(&fb_ptr, &width, &height, &linelen);
    fprintf(stderr, "Hello World! ret = %d\n", ret);

    initDisplayMemInfo(fb_ptr, (uint16_t)width, (uint16_t)height, linelen);
}

void deInitDisplay(void)
{
    deinitAdfDevice();
}

void displayFlush(void)
{
    AdfFlip();
}
