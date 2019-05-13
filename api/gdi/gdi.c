#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "adf_class.h"
#include "simple_draw.h"

#include "timer_api.h"

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


// use timer to reduce too fast flushing.
// delay a flushing if last flushing is in drawing.
// if more and more flushing be came very quickly, only the last one is activated.

static timer_id_t gui_flip_timer_id = -1;

#define DRAW_FREQ 13
#define ONE_TO_NANO (1000 * 1000 * 1000)
#define DRAW_PERIOD_IN_NS (ONE_TO_NANO / DRAW_FREQ + (ONE_TO_NANO % DRAW_FREQ != 0))
static struct timespec drawing_end_time;
static void start_drawing(void)
{
    AdfFlip();

    struct timespec drawing_begin_time;
    if (clock_gettime(CLOCK_MONOTONIC, &drawing_begin_time) < 0) {
        perror("clock_gettime");
        return;
    }

    // calc the end time
    drawing_end_time = drawing_begin_time;
    drawing_end_time.tv_nsec += DRAW_PERIOD_IN_NS;
    if (drawing_end_time.tv_nsec >= ONE_TO_NANO) {
        drawing_end_time.tv_nsec -= ONE_TO_NANO;
        drawing_end_time.tv_sec += 1;
    }
}

static void flip_timeout(timer_id_t id , uint64_t arg)
{
    start_drawing();
}


void displayFlush(void)
{
    if (gui_flip_timer_id < 0) {
        gui_flip_timer_id = createSimpleTimer(10000, true, flip_timeout);
    }
    struct timespec current;
    if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
        perror("clock_gettime");
        return;
    }
    if (current.tv_sec > drawing_end_time.tv_sec || (current.tv_sec == drawing_end_time.tv_sec && current.tv_nsec > drawing_end_time.tv_nsec)) {
        //fprintf(stderr, "Drawing at once\n");
        start_drawing();
    } else {
        //fprintf(stderr, "Drawing later\n");
        // should delay our flipping
        struct itimerspec itimerspec;
        itimerspec.it_value = drawing_end_time;
        itimerspec.it_interval.tv_sec = itimerspec.it_interval.tv_nsec = 0;
        modifyTimer(gui_flip_timer_id, 1, &itimerspec);

    }
}
