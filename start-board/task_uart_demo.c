#include "task_uart_demo.h"
#include "cmsis_os2.h"

#include "uart_io_api.h"

#include "serial_datagram.h"

#include <string.h>

__NO_RETURN static void uart_demo_thread(void *argument);

static const osThreadAttr_t thread_attr_uart_demo = {
	.name = "uart_demo_thread",
    .priority = osPriorityBelowNormal,
};

static osThreadId_t tid_uart_demo;

bool init_thread_of_uart_demo(void)
{
    tid_uart_demo = osThreadNew(uart_demo_thread, NULL, &thread_attr_uart_demo);
    return tid_uart_demo != NULL;
}

__NO_RETURN static void uart_demo_thread(void *argument)
{
    (void)argument;
    bool ret;
    uint32_t item[3] = { 0x12345678, 0xabcd};
    ret = serial_datagram_send(1, 1, item, 3);
    ret = serial_datagram_send(2, 1, item, 3);
    ret = ret;

    for(;;) {
        osDelay(osWaitForever);
    }
}

// Once a datagram be received from remote, this function will be invoked
// Parameters:
//   [in]seq, msg_id, data list and data count
void serial_datagram_arrived(const serial_datagram_item_t seq, const serial_datagram_item_t msg_id,
    const serial_datagram_item_t *restrict data_list, const size_t len)
{
    (void)seq;
    (void)msg_id;
    (void)data_list;
    (void)len;
    static uint32_t count;
    count++;
}
