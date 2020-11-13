#include "task_led_blink.h"

#include "gd32e10x.h"
#include "cmsis_os2.h"

__NO_RETURN static void led_blink_thread (void *argument)
{
    /* configure LED GPIO port */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_8);

    for(bool value = 0;; value ^= 1) {
        gpio_bit_write(GPIOA,GPIO_PIN_8, value);
        osDelay(200);
    }
}

static const osThreadAttr_t ThreadAttr_LED = {
   .name = "LED_Thread",
};


static osThreadId_t tid_led_blink;

bool init_thread_of_led_blink(void)
{
   tid_led_blink = osThreadNew(led_blink_thread, NULL, &ThreadAttr_LED);
   if (!tid_led_blink) {
       return false;
   }

   return true;
}
