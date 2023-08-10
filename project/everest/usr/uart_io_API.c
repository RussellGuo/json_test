// By Guo Qiang, XBD, Huaqin, 2020-11-19
// a interrupt dirven UART low-level driver

#include "uart_io_API.h"
#include "mhscpu.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>
//#include "run_info_result_desc.h"
#include "mcu-hw-common.h"

//define use uart0 or uart3
//uart0 used debug or download
//uart3 used connect ap
#define USE_UART0
//#define USE_UART3

#ifdef USE_UART0
#define UART0_TX_PIN                 GPIO_Pin_1
#define UART0_RX_PIN                 GPIO_Pin_0
#define UART0_GPIO_PORT              GPIOA
#else
#define UART3_TX_PIN                 GPIO_Pin_9
#define UART3_RX_PIN                 GPIO_Pin_8
#define UART3_GPIO_PORT              GPIOE
#endif

#ifdef USE_UART0
#define USART                       UART0
#define USART_IRQn                  UART0_IRQn
#else
#define USART                       UART3
#define USART_IRQn                  UART3_IRQn
#endif

#define UART_TX_PIN                 UART0_TX_PIN
#define UART_RX_PIN                 UART0_RX_PIN
#define UART_GPIO_PORT              UART0_GPIO_PORT

#define MAX_BYTE_RECV 136 // same as max datagram size
#define MAX_BYTE_SEND 136 // same as max datagram size

// A transmitted byte occupies the size of MQ.
// I try it and found it's 16.
#define MQ_DATAITEM_SIZE 16

__ALIGNED(8) static uint8_t mem_of_mq_recv[MAX_BYTE_RECV * MQ_DATAITEM_SIZE];
const static osMessageQueueAttr_t mq_attr_uart_recv = {
    .name = "mq_uart_recv",
    .mq_mem = mem_of_mq_recv,
    .mq_size = sizeof mem_of_mq_recv,
};

__ALIGNED(8) static uint8_t mem_of_mq_send[MAX_BYTE_SEND * MQ_DATAITEM_SIZE];
const static osMessageQueueAttr_t mq_attr_uart_send = {
    .name = "mq_uart_send",
    .mq_mem = mem_of_mq_send,
    .mq_size = sizeof mem_of_mq_send,
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
static void init_uart_controller(UART_TypeDef * uart_no, uint8_t uart_irq);

// init the UART
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void)
{
    mq_id_uart_recv = osMessageQueueNew(MAX_BYTE_RECV, sizeof(uint8_t), &mq_attr_uart_recv);
    mq_id_uart_send = osMessageQueueNew(MAX_BYTE_SEND, sizeof(uint8_t), &mq_attr_uart_send);
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
    osStatus_t status;
    for(;;) {
        status = osMessageQueueGet(mq_id_uart_recv, byte, NULL, delay);
        if (status != osErrorResource) {
            break;
        }

        // Temporarily unable to read the data, then wait and try again
        osDelay(1);
    }
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
        UART_ITConfig(USART, UART_IT_TX_EMPTY, ENABLE);
    }

    osMutexRelease(mutex_uart_sending_id);
    return ret;
}

static const struct mcu_pin_t uart_pin_tab[] = {
    {UART_GPIO_PORT, UART_TX_PIN, GPIO_Remap_0}, // USARTx_Tx
    {UART_GPIO_PORT, UART_RX_PIN, GPIO_Remap_0}, // USARTx_Rx
};

// pin define for UART
static void init_uart_pins_uart(void)
{
    /* connect port to USARTx_Tx */
    setup_pins(uart_pin_tab, sizeof(uart_pin_tab) / sizeof(uart_pin_tab[0]));
}

// controller configuration for UART
static void init_uart_controller(UART_TypeDef * uart_no, uint8_t uart_irq)
{
    /* USART configure */
    UART_InitTypeDef            UART_InitStructure;

    UART_InitStructure.UART_BaudRate	= 115200;
    UART_InitStructure.UART_WordLength	= UART_WordLength_8b;
    UART_InitStructure.UART_Parity		= UART_Parity_No;
    UART_InitStructure.UART_StopBits	= UART_StopBits_1;

    UART_Init(uart_no, &UART_InitStructure);
    UART_ITConfig(uart_no, UART_IT_RX_RECVD, ENABLE);
    NVIC_EnableIRQ(USART_IRQn);
    (void)uart_irq;
}

typedef enum
{
    //Clear to send or data set ready or ring indicator or data carrier detect.
    //Note that if auto flow control mode is enabled, a change in CTS(that is, DCTS set)does not cause an interrupt.
    MODEM_STATUS    = 0x00,
    //None(No interrupt occur)
    NONE            = 0x01,
    //Transmitter holding register empty(Prog.
    //THRE Mode disabled)or XMIT FIFO at or below threshold(Prog. THRE Mode enable)
    TRANS_EMPTY     = 0x02,
    //Receive data available or FIFO trigger level reached
    RECV_DATA       = 0x04,
    //Overrun/parity/framing errors or break interrupt
    RECV_STATUS     = 0x06,
    //UART_16550_COMPATIBLE = NO and master has tried to write to the
    //Line Control Register while the DW_apb_uart is busy(USR[0] is set to1)
    BUSY_DETECT     = 0x07,
    //No char in or out of RCVR FIFO during the last 4 character times
    //and there is at least 1 character in it during this time
    CHAR_TIMEOUT    = 0x0C
} INT_FLAG;

static inline void uart_irq(UART_TypeDef * uart_no)
{
    // TODO: record_uart_recv_error(1) if Frame Error/Noise Error/overrun be catched
    uint8_t byte;
    osStatus_t status;
    INT_FLAG int_Flag = (INT_FLAG)(uart_no->OFFSET_8.IIR & 0x0F);

    switch(int_Flag)
    {
    //Receive data available
    case RECV_DATA:
    //Recv data available but not reach the recv threshold
    case CHAR_TIMEOUT:
        byte = (uint8_t)UART_ReceiveData(uart_no);
        if (osMessageQueueGetSpace(mq_id_uart_recv) < 1) {
                uint8_t dropped_byte;
                osMessageQueueGet(mq_id_uart_recv, &dropped_byte, 0, 0);
                //record_uart_recv_dropped(1);
        }
        status = osMessageQueuePut(mq_id_uart_recv, &byte, 0, 0);
        if (status != osOK) {
            //record_uart_inner_error(1);
        } else {
            //record_uart_recv(1);
        }
        break;

    case TRANS_EMPTY:
        status = osMessageQueueGet(mq_id_uart_send, &byte, 0, 0);
        if (status == osOK) {
            uint32_t evt_set_ret = osEventFlagsSet(evt_flags_id_of_sending_queue_delivery, SEND_QUEUE_DELIVERY_FLAG);
            evt_set_ret = 0;
            UART_SendData(uart_no, byte);
        } else {
            UART_ITConfig(uart_no, UART_IT_TX_EMPTY, DISABLE);
        }
        break;
    case NONE:
        break;
    case RECV_STATUS:
        //Error recv
    default:
        break;
    }
}

#if defined(USE_UART3)
void UART3_IRQHandler(void)
{
    uart_irq(UART3);
}
#endif

#if defined(USE_UART0)
void UART0_IRQHandler(void)
{
    uart_irq(UART0);
}
#endif

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    if (ch == '\n')
    {
        fputc('\r', f);
    }
    while(!UART_IsTXEmpty(UART0));
    UART_SendData(UART0, (uint8_t) ch);
    return ch;
}
