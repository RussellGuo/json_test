#ifndef __UART_IO_API_H__
#define __UART_IO_API_H__

// By Guo Qiang, XBD, Huaqin, 2020-11-19
// a interrupt dirven UART low-level driver

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// init the UART(for now, it's USART2)
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void);

// receive one byte from UART(for now, it's USART2)
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, const uint32_t delay);

// NOT ready yet
// receive a line from UART(for now, it's USART2)
// a 'line' means a string ended by a '\r'
// parameters:
//  [out]bytes, ptr to store the received bytes
//  [in out] len, it's a ptr to store max size of the 'bytes' as input, and will keep the actual received size as output
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_line(uint8_t *bytes, size_t *len, uint32_t delay);

// send data into UART(for now, it's USART2)
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, const uint32_t delay);

// to override default ISR of USART2
void USART2_IRQHandler(void);

#endif //__UART_IO_API_H__
