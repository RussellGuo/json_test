#ifndef __UART_IO_API_H__
#define __UART_IO_API_H__

// By Guo Qiang, XBD, Huaqin, 2020-11-19
// a interrupt dirven UART low-level driver

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// init the UART
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void);

// receive one byte from UART
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, const uint32_t delay);

// send data into UART
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, const uint32_t delay);

// to override default ISR of USART2 and USART0
//void UART3_IRQHandler(void);
//void UART0_IRQHandler(void);

#endif //__UART_IO_API_H__
