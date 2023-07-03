#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mhscpu_uart.h"
#include <stdio.h>

void UART_Configuration(void);
int fputc(int ch, FILE *f);

#ifdef __cplusplus
}
#endif

#endif
