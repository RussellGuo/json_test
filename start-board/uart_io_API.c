#include "uart_io_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include <string.h>


// TODO: Support multiple UARTs

static osMessageQueueAttr_t mq_attr_uart_recv = {
    .name = "mq_uart_recv",
};

static osMessageQueueId_t mq_id_uart_recv;

static osMessageQueueAttr_t mq_attr_uart_send = {
    .name = "mq_uart_send",
};

// a message quque for uart receiving buffer, ISR will save data into it, and recv function will get data from it
static osMessageQueueId_t mq_id_uart_recv;
// a message quque for uart sending buffer, ISR will gey data from it, and send function will save data into it
static osMessageQueueId_t mq_id_uart_send;

// pin define for UART
static void init_uart_pins_uart2(void);
// controller configuration for UART
static void init_uart_controller(uint32_t uart_no, uint8_t uart_irq);

// init the UART(for now, it's USART2)
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void)
{
    mq_id_uart_recv = osMessageQueueNew(128, sizeof(uint8_t), &mq_attr_uart_recv);
    mq_id_uart_send = osMessageQueueNew(128, sizeof(uint8_t), &mq_attr_uart_send);

    bool ret = mq_id_uart_recv != NULL && mq_id_uart_send != NULL;
    if (ret) {
        init_uart_pins_uart2();
        init_uart_controller(USART2, USART2_IRQn);
    }

    return ret;
}

// receive one byte from UART(for now, it's USART2)
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, uint32_t delay)
{
    osStatus_t status = osMessageQueueGet(mq_id_uart_recv, byte, NULL, delay);
    return status == osOK;
}

// send data into UART(for now, it's USART2)
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, uint32_t delay)
{
    // TODO: limited delay time should be processed correctly; multipile invoking process

    (void)delay;

    if (buf == NULL) {
        return false;
    }
    if (size == 0) {
        return true;
    }

    if (osMessageQueueGetCapacity(mq_id_uart_send) < size) {
        return false;
    }

    for(;;) {
        if (osMessageQueueGetSpace(mq_id_uart_send) >= size) {
            break;
        }
        // TODO: Use Event-Flags to detect whether the buffer size is enough
        osDelay(1000);
    }

    bool ret = true;
    for (size_t i = 0; i < size; i++) {
        osStatus_t status = osMessageQueuePut(mq_id_uart_send, buf + i, 0, 0);
        if (status != osOK) {
            ret = false;
            break;
        }
    }

    // FIXME: USART2 is too ugly
    usart_interrupt_enable(USART2, USART_INT_TBE);
    return ret;
}


#define UART2_CLK                    RCU_USART2
#define UART2_TX_PIN                 GPIO_PIN_10
#define UART2_RX_PIN                 GPIO_PIN_11
#define UART2_GPIO_PORT              GPIOB
#define UART2_GPIO_CLK               RCU_GPIOB


// pin define for UART
static void init_uart_pins_uart2(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(UART2_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(UART2_CLK);

    /* connect port to USARTx_Tx */
    gpio_init(UART2_GPIO_PORT, GPIO_MODE_AF_PP      , GPIO_OSPEED_50MHZ, UART2_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(UART2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, UART2_RX_PIN);

}

// controller configuration for UART
static void init_uart_controller(uint32_t uart_no, uint8_t uart_irq)
{
    /* USART configure */
    usart_deinit(uart_no);
    usart_baudrate_set(uart_no, 115200U);
    usart_receive_config(uart_no, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart_no, USART_TRANSMIT_ENABLE);
    usart_enable(uart_no);

    nvic_irq_enable(uart_irq, 0, 0);
    usart_interrupt_enable(uart_no, USART_INT_RBNE);
}

static size_t uart_rx_dropped_count, uart_inner_error_count;

static inline void uart_irq(uint32_t uart_no)
{
    // TODO: Frame Error/Noise Error/overrun Error should be catched here
    if(RESET != usart_interrupt_flag_get(uart_no, USART_INT_FLAG_RBNE)){
        /* receive data */
        uint8_t byte = (uint8_t)usart_data_receive(uart_no);
        if (osMessageQueueGetSpace(mq_id_uart_recv) < 1) {
                uint8_t dropped_byte;
                osMessageQueueGet(mq_id_uart_recv, &dropped_byte, 0, 0);
                uart_rx_dropped_count++;
        }
        osStatus_t status = osMessageQueuePut(mq_id_uart_recv, &byte, 0, 0);
        if (status != osOK) {
            uart_inner_error_count++;
        }

    }
    if(RESET != usart_interrupt_flag_get(uart_no, USART_INT_FLAG_TBE)){
        /* transmit data */
        uint8_t byte;
        osStatus_t status = osMessageQueueGet(mq_id_uart_send, &byte, 0, 0);
        if (status == osOK) {
            usart_data_transmit(uart_no, byte);
        } else {
            usart_interrupt_disable(uart_no, USART_INT_TBE);
        }
    }
}

void USART2_IRQHandler(void)
{
    uart_irq(USART2);
}
