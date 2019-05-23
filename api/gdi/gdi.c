#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <pthread.h>

#include "adf_class.h"
#include "simple_draw.h"

#include "timer_api.h"

static pthread_mutex_t mutex_flushing = PTHREAD_MUTEX_INITIALIZER, mutex_drawing = PTHREAD_MUTEX_INITIALIZER;

static void *fbmem, *fbuser, *fbuser_cache;
static size_t fbmem_size;

static void init_flip_timer(void);
void initDisplay(void)
{
    uint32_t width, height, linelen;
    bool ret = initAdfDevice(&fbmem, &width, &height, &linelen);
    fprintf(stderr, "Hello World! ret = %d\n", ret);

    fbmem_size = height * linelen;
    fbuser = malloc(fbmem_size);
    fbuser_cache = malloc(fbmem_size);
    if (!ret || fbuser == NULL || fbuser_cache == NULL) {
        fprintf(stderr, "not enough memory for User FB and cache FB\n");
        return;
    }
    initDisplayMemInfo(fbuser, (uint16_t)width, (uint16_t)height, linelen);
    init_flip_timer();
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
static void start_drawing(const void *data)
{
    pthread_mutex_lock(&mutex_drawing);
    memcpy(fbmem, data, fbmem_size);
    AdfFlip();

    struct timespec drawing_begin_time;
    if (clock_gettime(CLOCK_MONOTONIC, &drawing_begin_time) < 0) {
        perror("clock_gettime");
    } else {
        // calc the end time
        drawing_end_time = drawing_begin_time;
        drawing_end_time.tv_nsec += DRAW_PERIOD_IN_NS;
        if (drawing_end_time.tv_nsec >= ONE_TO_NANO) {
            drawing_end_time.tv_nsec -= ONE_TO_NANO;
            drawing_end_time.tv_sec += 1;
        }
    }
    pthread_mutex_unlock(&mutex_drawing);
}

static void flip_timeout(timer_id_t id , uint64_t arg)
{
    start_drawing(fbuser_cache);
}

static void init_flip_timer(void)
{
    gui_flip_timer_id = createSimpleTimer(0, true, flip_timeout);
}
void displayFlush(void)
{
    pthread_mutex_lock(&mutex_flushing);
    struct timespec current;
    if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
        perror("clock_gettime");
    } else if (current.tv_sec > drawing_end_time.tv_sec || (current.tv_sec == drawing_end_time.tv_sec && current.tv_nsec > drawing_end_time.tv_nsec)) {
        // fprintf(stderr, "Drawing at once\n");
        start_drawing(fbuser);
    } else {
        memcpy(fbuser_cache, fbuser, fbmem_size);
        // should delay our flipping
        struct itimerspec itimerspec;
        itimerspec.it_value = drawing_end_time;
        itimerspec.it_interval.tv_sec = itimerspec.it_interval.tv_nsec = 0;
        modifyTimer(gui_flip_timer_id, 1, &itimerspec);

        // uint64_t nano_sec = (drawing_end_time.tv_sec  - current.tv_sec)* ONE_TO_NANO + (drawing_end_time.tv_nsec - current.tv_nsec);
        // fprintf(stderr, "%u ms left\n", (unsigned)nano_sec / 1000000);


    }
    pthread_mutex_unlock(&mutex_flushing);
}
