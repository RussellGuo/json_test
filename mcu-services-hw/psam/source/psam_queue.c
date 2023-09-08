#include "psam_queue.h"
#include "psam_type.h"

static int queue_fill(queue_t *queue, unsigned char *data, int length, int errno);
static int queue_tunnel(queue_t *queue, unsigned char *data, int length);

MOD_LOCAL void queue_init(queue_t *queue)
{
    queue->fill = queue_fill;
    queue->tunnel = queue_tunnel;
    queue->prcv_in = queue->buf;
    queue->prcv_out = queue->buf;
    queue->pxmt_in = queue->buf;
    queue->pxmt_out = queue->buf;
    queue->err = SC_ERR_NONE;
    //memset(queue->buf, 0, sizeof(QUEUE_SIZE));
}

static int queue_fill(queue_t *queue, unsigned char *data, int length, int errno)
{
    if (queue->err == SC_ERR_NONE)
        queue->err = errno;

    if (length > (QUEUE_SIZE - (queue->prcv_in - queue->buf))) {
        length = (QUEUE_SIZE - (queue->prcv_in - queue->buf));
        queue->err = SC_ERR_PARAM;
    }
    memcpy(queue->prcv_in, data, length);
    queue->prcv_in += length;

    return length;
}

MOD_LOCAL int queue_rcv(queue_t *queue, unsigned char *rcv, int length)
{
    int errno = SC_ERR_NONE;

    if (length <= 0)
        return SC_ERR_NONE;

    while ((queue->prcv_in - queue->prcv_out) < length) {
        if (queue->err != SC_ERR_NONE) {
            if (queue->err == SC_ERR_BYTE_PARITY) {
                errno = queue->err;
                break;
            } else {
                return queue->err;
            }
        }
        // else wait for fill in hw callback fill
    }

    memcpy(rcv, queue->prcv_out, length);
    queue->prcv_out += length;
    if (queue->err != SC_ERR_NONE) {
        errno = queue->err;
        queue->err = SC_ERR_NONE;
    }

    return errno;
}

static int queue_tunnel(queue_t *queue, unsigned char *buf, int length)
{
    if (buf != NULL) {
        if ((queue->pxmt_in - queue->pxmt_out) <= length) {
            length = queue->pxmt_in - queue->pxmt_out;
        }
        memcpy(buf, queue->pxmt_out, length);
        queue->pxmt_out += length;
        return length;
    } else {
        // if buf==NULL, return number of data in queue
        return (queue->pxmt_in - queue->pxmt_out);
    }
}

MOD_LOCAL void queue_xmt(queue_t *queue, unsigned char *xmt, int length)
{
    if ((QUEUE_SIZE - (queue->pxmt_in - queue->buf)) >= length) {
        memcpy(queue->pxmt_in, xmt, length);
        queue->pxmt_in += length;
    }
}


MOD_LOCAL void queue_flush(queue_t *queue)
{
    queue->prcv_in = queue->buf;
    queue->prcv_out = queue->buf;
    queue->pxmt_in = queue->buf;
    queue->pxmt_out = queue->buf;
    queue->err = SC_ERR_NONE;
    //memset(queue->buf, 0, sizeof(QUEUE_SIZE));
}
