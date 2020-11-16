#include "user_key_isr.h"
#include "gd32e10x.h"


static user_key_callback_t user_key_cb;

// ISR configuration function
// enable is none-zero: enbale it, otherwise disable it
static void enable_irq_of_key(bool enable);

// cb != NULL, a callback of the key interrupt.
// cb == NULL, uninstall the last init
// return non-zero if succeeded
bool init_user_key_isr(user_key_callback_t cb)
{
    if (user_key_cb != 0 && cb != 0) {
        return false;
    }

    user_key_cb = cb;
    enable_irq_of_key(user_key_cb != 0);
    return true;
}


static void enable_irq_of_key(bool enable)
{
    /* enable the key clock */
    rcu_periph_clock_enable(USER_KEY_GPIO_CLK);
    rcu_periph_clock_enable(RCU_AF);

    /* configure key pin as input */
    gpio_init(USER_KEY_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USER_KEY_PIN);

    if (enable) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(USER_KEY_EXTI_IRQn, 2U, 0U);
    } else {
        nvic_irq_disable(USER_KEY_EXTI_IRQn);
    }

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(USER_KEY_EXTI_PORT_SOURCE, USER_KEY_EXTI_PIN_SOURCE);

    /* configure key EXTI line */
    exti_init(USER_KEY_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);

    return;
}

// to avoid compiler warning
void EXTI0_IRQHandler(void);

// system has a default weak IRQ handler
// this definition will override that.
void EXTI0_IRQHandler(void)
{
    exti_interrupt_flag_clear(EXTI_0);
    if (user_key_cb) {
        bool pressed = gpio_input_bit_get(USER_KEY_GPIO_PORT, USER_KEY_PIN) != 0;
        user_key_cb(pressed);
    }
}

