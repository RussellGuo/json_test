#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "uart_io_api.h"
#include "i2c-base.h"

#include "task_serial_datagram_recv.h"

#include "service_led.h"
#include "service_laser.h"
#include "service_flashlight.h"

#include "misc.h"

__NO_RETURN int main(void)
{
    osKernelInitialize();
    NVIC_SetPriorityGrouping (3);                    // setup priority grouping

    set_rpc_log_level(LOG_FATAL);

    init_uart_io_api();
    init_i2c_buses();

    init_thread_of_serial_datagram_recv();

    init_thread_of_service_led();
    init_thread_of_service_laser();
    init_thread_of_service_flashlight();

    osKernelStart();                                 // start OS
    while(1);
}
