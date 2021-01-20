/*
 * task_serial_datagram_recv.h
 *
 * task of protocol datagram receiving
 * this task is the main task of the MCU side, and should be start at main()
 *
 *  Created on: Nov 25, 2020
 *      Author: Guo Qiang
 */

#include "task_serial_datagram_recv.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "serial_datagram.h"

// stack size definition
#define STACK_SIZE_OF_RECV_THREAD 1024
__ALIGNED(8) static uint8_t stack_of_thread[STACK_SIZE_OF_RECV_THREAD];

// task thread attribute
static const osThreadAttr_t thread_attr_serial_datagram_recv = {
    .name = "serial_datagram_recv_thread",
    .priority = osPriorityRealtime4,
    .stack_mem  = stack_of_thread,
    .stack_size = sizeof(stack_of_thread),
};

// task thread ID
static osThreadId_t tid_serial_datagram_recv;

// start task of protocol datagram receiving
// parameter: none
// return value: true if done; otherwise failed
bool init_thread_of_serial_datagram_recv(void)
{
    tid_serial_datagram_recv = osThreadNew(serial_datagram_receive_loop, NULL, &thread_attr_serial_datagram_recv);
    return tid_serial_datagram_recv != NULL;
}
