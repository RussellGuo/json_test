#include "task_uart_demo.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

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

static void init_uart(void);

__NO_RETURN static void uart_demo_thread(void *argument)
{
    (void)argument;
    init_uart();


    for(;;) {
        osDelay(osWaitForever);
    }
}



#define UART_NO                     USART2
#define UART_CLK                    RCU_USART2
#define UART_TX_PIN                 GPIO_PIN_10
#define UART_RX_PIN                 GPIO_PIN_11
#define UART_GPIO_PORT              GPIOB
#define UART_GPIO_CLK               RCU_GPIOB


static void init_uart(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(UART_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_AF_PP      , GPIO_OSPEED_50MHZ, UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, UART_RX_PIN);

    /* USART configure */
    usart_deinit(UART_NO);
    usart_baudrate_set(UART_NO, 115200U);
    usart_receive_config(UART_NO, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_NO, USART_TRANSMIT_ENABLE);
    usart_enable(UART_NO);
    
    nvic_irq_enable(USART2_IRQn, 0, 0);
    usart_interrupt_enable(USART2, USART_INT_RBNE);
    usart_interrupt_enable(USART2, USART_INT_TBE);
    
}

void USART2_IRQHandler(void)
{
    static int idx;
    static uint16_t recv_char;
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)){
        /* receive data */
        recv_char = usart_data_receive(USART2);
    }
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_TBE)){
        /* transmit data */
        char c = 0;

       if (recv_char) {
           c = (char) recv_char;
           recv_char = 0;
       } else {
           // otherwise, output a pre-defined string
            const char *msg = "Hello World!\r";
            c = msg[idx];
            if (c != 0) {
                idx++;
            } else {
                idx = 0;
                c = '\n';
            }
       }
       usart_data_transmit(UART_NO, c);
   }
}
