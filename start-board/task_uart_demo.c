#include "task_uart_demo.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include <string.h>

__NO_RETURN static void uart_demo_thread(void *argument);

static const osThreadAttr_t thread_attr_uart_demo = {
	.name = "uart_demo_thread", //This decleration requires C99
    .priority = osPriorityBelowNormal,
};

static osThreadId_t tid_uart_demo;

bool init_thread_of_uart_demo(void)
{
   tid_uart_demo = osThreadNew(uart_demo_thread, NULL, &thread_attr_uart_demo);
   return tid_uart_demo != NULL;
}



static void init_uart(void);
#define UART_NO                     USART2
#define UART_CLK                    RCU_USART2
#define UART_TX_PIN                 GPIO_PIN_10
#define UART_RX_PIN                 GPIO_PIN_11
#define UART_GPIO_PORT              GPIOB
#define UART_GPIO_CLK               RCU_GPIOB

__NO_RETURN static void uart_demo_thread(void *argument)
{
    (void)argument;
    init_uart();


    for(int i = 0;;) {
        char c;

       if (RESET != usart_flag_get(UART_NO, USART_FLAG_RBNE)){
            // echo if got a char
           /* receive data */
           c = (char) usart_data_receive(UART_NO);
       } else {
           // otherwise, output a pre-defined string
            const char *msg = "Hello World!\r\n";
            c = msg[i];
            if (c != 0) {
                i++;
            } else {
                i = 0;
                continue;
            }
       }

        usart_data_transmit(UART_NO, c);
        osDelay(50);
    }
}


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
}
