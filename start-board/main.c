#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "uart_io_api.h"

#include "task_serial_datagram_recv.h"

#include "service_led.h"

__NO_RETURN int main(void)
{
    osKernelInitialize();
    NVIC_SetPriorityGrouping (3);                    // setup priority grouping

    init_uart_io_api();

    init_thread_of_serial_datagram_recv();

    init_thread_of_service_led();
    osKernelStart();                                 // start RTX kernel
    while(1);
}
