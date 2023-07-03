#ifndef __BT42_UART_H
#define __BT42_UART_H

#include <stdio.h>
#include <stdint.h>


void host_uart_send(uint8_t *pbuff, uint16_t len);
uint16_t host_uart_read(uint8_t *pbuff, uint16_t len);
#endif
