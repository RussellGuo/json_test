#include "uart_io_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

#include <string.h>

#define UART2_TX_PIN                 GPIO_PIN_10
#define UART2_RX_PIN                 GPIO_PIN_11
#define UART2_GPIO_PORT              GPIOB
#define UART2_GPIO_CLK               RCU_GPIOB

#define UART0_TX_PIN                 GPIO_PIN_9
#define UART0_RX_PIN                 GPIO_PIN_10
#define UART0_GPIO_PORT              GPIOA
#define UART0_GPIO_CLK               RCU_GPIOA

#if defined(GD32E103R_START)

#define USART                       USART2
#define USART_IRQn                  USART2_IRQn

#define UART_CLK                    RCU_USART2
#define UART_TX_PIN                 UART2_TX_PIN
#define UART_RX_PIN                 UART2_RX_PIN
#define UART_GPIO_PORT              UART2_GPIO_PORT
#define UART_GPIO_CLK               UART2_GPIO_CLK

#elif defined(SAIP_BOARD)

#define USART                       USART0
#define USART_IRQn                  USART0_IRQn

#define UART_CLK                    RCU_USART0
#define UART_TX_PIN                 UART0_TX_PIN
#define UART_RX_PIN                 UART0_RX_PIN
#define UART_GPIO_PORT              UART0_GPIO_PORT
#define UART_GPIO_CLK               UART0_GPIO_CLK

#else

#error "for now, we support START board and SAIP_BOARD only"

#endif

// TODO: Support multiple UARTs

const static osMessageQueueAttr_t mq_attr_uart_recv = {
    .name = "mq_uart_recv",
};

const static osMessageQueueAttr_t mq_attr_uart_send = {
    .name = "mq_uart_send",
};

// a message quque for uart receiving buffer, ISR will save data into it, and recv function will get data from it
static osMessageQueueId_t mq_id_uart_recv;
// a message quque for uart sending buffer, ISR will gey data from it, and send function will save data into it
static osMessageQueueId_t mq_id_uart_send;

#define SEND_QUEUE_DELIVERY_FLAG 2
const static osEventFlagsAttr_t evt_flags_attr_of_sending_queue_delivery = {
    .name = "evt_flags_of_sending_queue_changed",
};

static osEventFlagsId_t evt_flags_id_of_sending_queue_delivery; // ISR notifies sending function


// a mutex for sending
static osMutexId_t mutex_uart_sending_id;
static const osMutexAttr_t mutex_uart_sending_attr = {
    .name = "uart_sending_mutex",                          // human readable mutex name
    .attr_bits = osMutexRecursive | osMutexPrioInherit,    // attr_bits
};

// pin define for UART
static void init_uart_pins_uart(void);
// controller configuration for UART
static void init_uart_controller(uint32_t uart_no, uint8_t uart_irq);

// init the UART
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void)
{
    mq_id_uart_recv = osMessageQueueNew(128, sizeof(uint8_t), &mq_attr_uart_recv);
    mq_id_uart_send = osMessageQueueNew(128, sizeof(uint8_t), &mq_attr_uart_send);
    evt_flags_id_of_sending_queue_delivery = osEventFlagsNew(&evt_flags_attr_of_sending_queue_delivery);
    mutex_uart_sending_id = osMutexNew (&mutex_uart_sending_attr);

    bool ret = mq_id_uart_recv != NULL && mq_id_uart_send != NULL && evt_flags_id_of_sending_queue_delivery != NULL && mutex_uart_sending_id != NULL;
    if (ret) {
        init_uart_pins_uart();
        init_uart_controller(USART, USART_IRQn);
    }

    return ret;
}

// receive one byte from UART
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, const uint32_t delay)
{
    osStatus_t status = osMessageQueueGet(mq_id_uart_recv, byte, NULL, delay);
    return status == osOK;
}

// send data into UART
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, const uint32_t delay)
{
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

    bool ret = false;
    uint32_t begin_tick = osKernelGetTickCount();

    if (osMutexAcquire(mutex_uart_sending_id, delay) != osOK) {
        return false;
    }

    for(;;) {
        if (osMessageQueueGetSpace(mq_id_uart_send) >= size) {
            ret = true;
            break;
        }

        // to calc the left delay time
        uint32_t delay_from_now;
        if (delay == osWaitForever) {
            // forever
            delay_from_now = osWaitForever;
        } else {
            uint32_t now = osKernelGetTickCount();
            if (now >= begin_tick + delay) {
                // already time out
                ret = false;
                break;
            } else {
                // a smaller delay time
                delay_from_now = delay + begin_tick - now;
            }
        }
        uint32_t evt_flags_wait_ret = osEventFlagsWait(evt_flags_id_of_sending_queue_delivery, SEND_QUEUE_DELIVERY_FLAG, osFlagsWaitAny, delay_from_now);
        if (evt_flags_wait_ret == (uint32_t)osErrorTimeout) {
            ret = false;
            break;
        }
    }

    if (ret) {
        for (size_t i = 0; i < size; i++) {
            osStatus_t status = osMessageQueuePut(mq_id_uart_send, buf + i, 0, 0);
            if (status != osOK) {
                // I think it's never be reached
                ret = false;
                break;
            }
        }
        usart_interrupt_enable(USART, USART_INT_TBE);
    }

    osMutexRelease(mutex_uart_sending_id);
    return ret;
}



// pin define for UART
static void init_uart_pins_uart(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(UART_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_AF_PP      , GPIO_OSPEED_50MHZ, UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, UART_RX_PIN);

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
            uint32_t evt_set_ret = osEventFlagsSet(evt_flags_id_of_sending_queue_delivery, SEND_QUEUE_DELIVERY_FLAG);
            evt_set_ret = 0;
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

void USART0_IRQHandler(void)
{
    uart_irq(USART0);
}
