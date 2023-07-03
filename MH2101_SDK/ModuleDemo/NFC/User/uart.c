#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "mhscpu.h"

uart_param g_uart_param;

//| Stop Bit 1 | Data Bit 8 |
#define LCR_DATA8_STOP1 (0x03)

void uart_Config(uint32_t baudrate, uint32_t parity)
{
    UART_InitTypeDef uart;

    uart.UART_BaudRate = baudrate; 
    uart.UART_Parity = parity;
    uart.UART_StopBits = UART_StopBits_1;
    uart.UART_WordLength = UART_WordLength_8b;
    UART_Init(UART0, &uart);
    memset(&g_uart_param, 0, sizeof(g_uart_param));
    
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
    //Set Enable FIFO Receive Threshold 1/2 Send Threshold 2 characters in the FIFO
    UART0->OFFSET_8.FCR = (2 << 6) | (1 << 4) | BIT2 | BIT1 | BIT0;
    UART_ITConfig(UART0, RECV_INT, ENABLE);
    NVIC_EnableIRQ(UART0_IRQn);
}

int32_t uart_IsSendFinish(void)
{
    return ((UART0->LSR & BIT6) && g_uart_param.send.bytes < 1);
}

void uart_RecvFlush(void)
{
    //disable recv interrupt.
    UART_ITConfig(UART0, RECV_INT, DISABLE);
	
    //Reset recv FIFO
    UART0->OFFSET_8.FCR |= BIT1;
    memset(&g_uart_param.recv, 0, sizeof(g_uart_param.recv));
    UART_ITConfig(UART0, RECV_INT, ENABLE);
}

int32_t uart_SendChar(uint8_t ch)
{
    while (g_uart_param.send.bytes == UART_SEND_BUF_SIZE)
    {
    }
	
    g_uart_param.send.buf[g_uart_param.send.write_index++] = ch;
    if (g_uart_param.send.write_index >= UART_SEND_BUF_SIZE)
    {
        g_uart_param.send.write_index = 0;
    }
    UART_ITConfig(UART0, SEND_INT, DISABLE);
        g_uart_param.send.bytes++;    
    UART_ITConfig(UART0, SEND_INT, ENABLE);

    return 0;
}

int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len)
{
    while (len--)
    {
        uart_SendChar(*pBuf++);
    }
    return 0;
}

int32_t uart_RecvChar(void)
{
    unsigned char c;
    
    if (g_uart_param.recv.bytes)
    {
        c = g_uart_param.recv.buf[g_uart_param.recv.read_index];
        UART_ITConfig(UART0, RECV_INT, DISABLE);
        if (++g_uart_param.recv.read_index >= UART_RECV_BUF_SIZE)
        {
            g_uart_param.recv.read_index = 0;
        }
        g_uart_param.recv.bytes--;
        UART_ITConfig(UART0, RECV_INT, ENABLE);
        return c;
    }
    return -1;
}

int32_t uart_getbytes(void)
{
    return g_uart_param.recv.bytes;
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


void UART0_IRQHandler(void)
{
    uint8_t i;
    INT_FLAG int_Flag = (INT_FLAG)(UART0->OFFSET_8.IIR & 0x0F);

    switch(int_Flag)
    {
    //Receive data available
    case RECV_DATA:
    //Recv data available but not reach the recv threshold 
    case CHAR_TIMEOUT:
        for (i = 0; i < 16 && UART0->RFL != 0; i++)
        {
            g_uart_param.recv.buf[g_uart_param.recv.write_index++] = UART0->OFFSET_0.RBR;
            if (g_uart_param.recv.write_index >= UART_RECV_BUF_SIZE)
            {
                g_uart_param.recv.write_index = 0;
            }
            if (g_uart_param.recv.bytes < UART_RECV_BUF_SIZE)
            {
                g_uart_param.recv.bytes++;
            }
            else
            {
                g_uart_param.recv.overflow = 1;
                g_uart_param.recv.read_index++;
                if (g_uart_param.recv.read_index >= UART_RECV_BUF_SIZE)
                {
                    g_uart_param.recv.read_index = 0;
                }
            }
        }
        break;
		
    case TRANS_EMPTY:
        do
        {
            if (!g_uart_param.send.bytes)
            {
                UART_ITConfig(UART0, SEND_INT, DISABLE);
                break;
            }
            UART0->OFFSET_0.THR = g_uart_param.send.buf[g_uart_param.send.read_index++];
            if (g_uart_param.send.read_index >= UART_SEND_BUF_SIZE)
            {
                g_uart_param.send.read_index = 0;
            }
            g_uart_param.send.bytes--;
        }while (UART0->TFL < 16);
        break;
        
    case NONE:
        break;
	
    case RECV_STATUS:
        //Error recv
        default:
        break;
    }
}

int32_t uart_SendCharPoll(uint8_t ch)
{
    while(0 == UART_IsTXEmpty(UART0));
    UART0->OFFSET_0.THR = ch;
   //while(0 == UART_IsTXEmpty(UART0));
    
    return 0;
}
