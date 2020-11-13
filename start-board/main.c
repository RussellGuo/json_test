#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "task_led_blink.h"

__NO_RETURN int main(void)
{
    osKernelInitialize();
    NVIC_SetPriorityGrouping (3);                    // setup priority grouping

    init_thread_of_led_blink();
    osKernelStart();                                 // start RTX kernel
}
