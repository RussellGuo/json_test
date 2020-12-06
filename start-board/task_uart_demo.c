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
    for(;;) {
        osDelay(osWaitForever);
    }
}

