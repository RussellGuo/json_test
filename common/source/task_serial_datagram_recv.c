#include "task_serial_datagram_recv.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "serial_datagram.h"

__ALIGNED(8) static uint8_t stack_of_thread[1024];
static const osThreadAttr_t thread_attr_serial_datagram_recv = {
    .name = "serial_datagram_recv_thread",
    .priority = osPriorityRealtime4,
    .stack_mem  = stack_of_thread,
    .stack_size = sizeof(stack_of_thread),
};

static osThreadId_t tid_serial_datagram_recv;

bool init_thread_of_serial_datagram_recv(void)
{
    tid_serial_datagram_recv = osThreadNew(serial_datagram_receive_loop, NULL, &thread_attr_serial_datagram_recv);
    return tid_serial_datagram_recv != NULL;
}
