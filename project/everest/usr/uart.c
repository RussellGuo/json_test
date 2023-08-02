#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "mhscpu.h"

#define RECV_INT  (BIT0)
#define SEND_INT  (BIT1 | BIT7)

void uart_Config(uint32_t baudrate)
{
    UART_InitTypeDef uart;

    uart.UART_BaudRate = baudrate;
    uart.UART_Parity = UART_Parity_No;
    uart.UART_StopBits = UART_StopBits_1;
    uart.UART_WordLength = UART_WordLength_8b;
    UART_Init(UART0, &uart);

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

    //Set Enable FIFO Receive Threshold 1/2 Send Threshold 2 characters in the FIFO
    UART0->OFFSET_8.FCR = (2 << 6) | (1 << 4) | BIT2 | BIT1 | BIT0;
    UART_ITConfig(UART0, RECV_INT, ENABLE);
    NVIC_EnableIRQ(UART0_IRQn);
}

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
