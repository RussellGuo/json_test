#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "uart_io_api.h"
#include "i2c-base.h"

#include "task_serial_datagram_recv.h"

#include "service_led.h"
#include "service_laser.h"
#include "service_flashlight.h"

#include "misc.h"
#include "db9_init_for_factory.h"

#include "service_camera.h"


__NO_RETURN int main(void)
{
    // OS initialization
    osKernelInitialize();
    NVIC_SetPriorityGrouping(3);  // setup priority grouping

    set_rpc_log_level(LOG_INFO);

    // drivers initialization
    init_uart_io_api();
    init_i2c_buses();

    camera_hw_init();
    db9_init_for_factory();

    // protocol base initialization
    init_thread_of_serial_datagram_recv();

    // services initialization
    init_thread_of_service_led();
    init_thread_of_service_laser();
    init_thread_of_service_flashlight();
    init_thread_of_service_laser_status();

    // let's go!
    osKernelStart();
    while(1);
}
