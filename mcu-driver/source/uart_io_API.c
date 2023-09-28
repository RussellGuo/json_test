// By Guo Qiang, XBD, Huaqin, 2020-11-19
// a interrupt dirven UART low-level driver

//Update:
// By Cao Meng, XBD, Huaqin, 2023-08-11
// adapter mh2101 mcu UART driver

#include "uart_io_API.h"
#include "mhscpu.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>
//#include "run_info_result_desc.h"

#define MAX_BYTE_RECV 136 // same as max datagram size
#define MAX_BYTE_SEND 136 // same as max datagram size

// A transmitted byte occupies the size of MQ.
// I try it and found it's 16.
#define MQ_DATAITEM_SIZE 16

__ALIGNED(8) static uint8_t mem_of_mq_recv[MAX_BYTE_RECV * MQ_DATAITEM_SIZE];
const static osMessageQueueAttr_t mq_attr_uart_recv = {
    .name = "mq_uart_recv",
    .mq_mem = mem_of_mq_recv,
    .mq_size = sizeof mem_of_mq_recv,
};

__ALIGNED(8) static uint8_t mem_of_mq_send[MAX_BYTE_SEND * MQ_DATAITEM_SIZE];
const static osMessageQueueAttr_t mq_attr_uart_send = {
    .name = "mq_uart_send",
    .mq_mem = mem_of_mq_send,
    .mq_size = sizeof mem_of_mq_send,
};

// a message quque for uart receiving buffer, ISR will save data into it, and recv function will get data from it
static osMessageQueueId_t mq_id_uart_recv;
// a message quque for uart sending buffer, ISR will gey data from it, and send function will save data into it
static osMessageQueueId_t mq_id_uart_send;

#define SEND_QUEUE_DELIVERY_FLAG 2
const static osEventFlagsAttr_t evt_flags_attr_of_sending_queue_delivery = {
    .name = "evt_flags_of_sending_queue_changed",
};

static osEventFlagsId_t evt_flags_id_of_sending_queue_delivery; // ISR notifies sending function


// a mutex for sending
static osMutexId_t mutex_uart_sending_id;
static const osMutexAttr_t mutex_uart_sending_attr = {
    .name = "uart_sending_mutex",                          // human readable mutex name
    .attr_bits = osMutexRecursive | osMutexPrioInherit,    // attr_bits
};


static bool store_recv_byte(uint8_t byte)
{
    osStatus_t status = osMessageQueuePut(mq_id_uart_recv, &byte, 0, 0);
    return status == osOK;
}

static bool store_recv_bytes(const uint8_t *bytes, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        bool ret = store_recv_byte(bytes[i]);
        if (!ret) {
            return false;
        }
    }
    return true;
}


// init the UART
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void)
{
    mq_id_uart_recv = osMessageQueueNew(MAX_BYTE_RECV, sizeof(uint8_t), &mq_attr_uart_recv);
    mq_id_uart_send = osMessageQueueNew(MAX_BYTE_SEND, sizeof(uint8_t), &mq_attr_uart_send);
    evt_flags_id_of_sending_queue_delivery = osEventFlagsNew(&evt_flags_attr_of_sending_queue_delivery);
    mutex_uart_sending_id = osMutexNew (&mutex_uart_sending_attr);

    bool ret = mq_id_uart_recv != NULL && mq_id_uart_send != NULL && evt_flags_id_of_sending_queue_delivery != NULL && mutex_uart_sending_id != NULL;
    const static uint8_t bin[] = {0xAAu, 8, 1, 50, 2, 8, 1, 0xCCu, 0xABu};
    ret &= store_recv_bytes(bin, sizeof bin);

	
    return ret;
}

// receive one byte from UART
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, const uint32_t delay)
{
    osStatus_t status;
    for(;;) {
        status = osMessageQueueGet(mq_id_uart_recv, byte, NULL, delay);
        if (status != osErrorResource) {
            break;
        }

        // Temporarily unable to read the data, then wait and try again
        osDelay(1);
    }
    return status == osOK;
}

// send data into UART
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, const uint32_t delay)
{
    (void)delay;

    if (buf == NULL) {
        return false;
    }
    if (size == 0) {
        return true;
    }

    if (osMessageQueueGetCapacity(mq_id_uart_send) < size) {
        return false;
    }

    bool ret = true;
    uint32_t begin_tick = osKernelGetTickCount();

    if (osMutexAcquire(mutex_uart_sending_id, delay) != osOK) {
        return false;
    }

    osMutexRelease(mutex_uart_sending_id);
    return ret;
}
