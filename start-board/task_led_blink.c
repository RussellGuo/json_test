#include "task_led_blink.h"
#include "user_key_isr.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"


#define KEY_PRESSED_FLAG 1
static void led_blink_until_key_pressed(uint16_t half_period)
{
    for(bool value = 0;; value ^= 1) {
        gpio_bit_write(LED_GPIO_PORT, LED_GPIO_PIN, value ? SET : RESET);
        uint32_t wait = osThreadFlagsWait(KEY_PRESSED_FLAG, osFlagsWaitAny, half_period);
        if (wait == KEY_PRESSED_FLAG)
            break;
    }
}

__NO_RETURN static void led_blink_thread(void *argument)
{
    (void)argument;

    /* configure LED GPIO port */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,LED_GPIO_PIN);

    for(;;) {
        led_blink_until_key_pressed( 512);
        led_blink_until_key_pressed( 256);
        led_blink_until_key_pressed( 128);
        led_blink_until_key_pressed(  64);
    }
}

static const osThreadAttr_t ThreadAttr_LED = {
   .name = "LED_Thread",
};


static osThreadId_t tid_led_blink;

static void key_pressed_notifier(bool pressed);

bool init_thread_of_led_blink(void)
{
   init_user_key_isr(key_pressed_notifier);
   tid_led_blink = osThreadNew(led_blink_thread, NULL, &ThreadAttr_LED);
   return tid_led_blink !=NULL;
}

static void key_pressed_notifier(bool pressed)
{
    if (pressed && tid_led_blink != NULL)
        osThreadFlagsSet(tid_led_blink, KEY_PRESSED_FLAG);
}
