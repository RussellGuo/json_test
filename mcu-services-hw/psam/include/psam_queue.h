#ifndef __SC_QUEUE_H__
#define __SC_QUEUE_H__

#include "makefun.h"

#define	QUEUE_SIZE      267

struct QUEUE;
typedef struct QUEUE queue_t;
struct QUEUE {
    volatile int err;
    unsigned char * volatile prcv_in;
    unsigned char * volatile prcv_out;
    unsigned char * volatile pxmt_in;
    unsigned char * volatile pxmt_out;
    int (*fill)(queue_t*, unsigned char*, int, int);
    int (*tunnel)(queue_t*, unsigned char*, int);
    unsigned char buf[QUEUE_SIZE];
};

MOD_LOCAL void queue_init(queue_t *queue);
MOD_LOCAL int queue_rcv(queue_t *queue, unsigned char *rcv, int length);
MOD_LOCAL void queue_xmt(queue_t *queue, unsigned char *xmt, int length);
MOD_LOCAL void queue_flush(queue_t *queue);

#endif //__SC_QUEUE_H__

