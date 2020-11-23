#include "task_uart_demo.h"
#include "cmsis_os2.h"

#include "uart_io_api.h"

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

// test UART sending pending
// sending 2 long string will cause pending,
// then enter the debug mode, and watch evt_flags_id_of_sending_queue_delivery related statments at the file 'uart_io_API.c'
static void test_sending_wait(void)
{
    uint8_t buf[96];
    for (size_t i = 0; i < sizeof buf; i++) {
        buf[i] = (uint8_t)(' ' + i);
    }
    buf[94] = '\r'; buf[95] = '\n';
    uart_send_data(buf, sizeof buf, osWaitForever);
    uart_send_data(buf, sizeof buf, osWaitForever);
}

__NO_RETURN static void uart_demo_thread(void *argument)
{
    (void)argument;
    test_sending_wait();

    for(;;) {
        uint8_t byte;
        bool ret = uart_recv_byte(&byte, osWaitForever);
        if (!ret) {
            break;
        }
        uart_send_data(&byte, 1, osWaitForever);
        if (byte == '\r') {
            uart_send_data((uint8_t *)"\n", 1, osWaitForever);
        }
    }
    while(1);
}

