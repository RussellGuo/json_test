#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "mhscpu.h"
#include "define.h"


#define RECV_INT  (BIT0)
#define SEND_INT  (BIT1 | BIT7)

uint8_t au8SendBuff[UART_SEND_BUF_SIZE + sizeof(RNG_BUF) + 16];
uint8_t au8RecvBuff[UART_RECV_BUF_SIZE + sizeof(RNG_BUF) + 16];


RNG_BUF_ID  uart_SendBuf = NULL;
RNG_BUF_ID  uart_RecvBuf = NULL;


//| Stop Bit 1 | Data Bit 8 |
#define LCR_DATA8_STOP1 (0x03)

static uint32_t uart_dll;

void uart_Config(uint32_t baudrate)
{
    UART_InitTypeDef            UART_InitStructure;
    UART_FIFOInitTypeDef        UART_FIFOInitStruct;

	UART_StructInit(&UART_InitStructure);
	UART_FIFOStructInit(&UART_FIFOInitStruct);


	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
    memset(au8SendBuff, 0x00, sizeof(au8SendBuff));
    memset(au8RecvBuff, 0x00, sizeof(au8RecvBuff));
    
    uart_SendBuf = (RNG_BUF_ID)au8SendBuff;
    uart_RecvBuf = (RNG_BUF_ID)au8RecvBuff;
    
    uart_SendBuf->head = 0;
    uart_SendBuf->tail = 1;
    uart_SendBuf->err_stat = 0;
    uart_SendBuf->buf_size = UART_SEND_BUF_SIZE;

    uart_RecvBuf->head = 0;
    uart_RecvBuf->tail = 1;
    uart_RecvBuf->err_stat = 0;
    uart_RecvBuf->buf_size = UART_RECV_BUF_SIZE;
	
	UART_InitStructure.UART_BaudRate	= baudrate;
	UART_InitStructure.UART_WordLength	= UART_WordLength_8b;
	UART_InitStructure.UART_Parity		= UART_Parity_No;
	UART_InitStructure.UART_StopBits	= UART_StopBits_1;
    
    UART_FIFOInitStruct.FIFO_Enable = ENABLE;
	UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_1;
	UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_1_4_Full;
	UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = ENABLE;
	
	UART_Init(UART0, &UART_InitStructure);
    UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);
    UART_FIFOInit(UART0, &UART_FIFOInitStruct);
}



static void uart_Resume(void)
{
    uint32_t en;

    //Disable int8 UART0 interrupt.
    NVIC->ICER[0] = BIT8;

    en = UART0->OFFSET_4.IER;

    //Reset UART
    UART0->SRR = 0x07;

    //Set Baud Rate Set LCR[7] bit = 1 DLAB Bit
    UART0->LCR |= BIT7;
    //UART0->OFFSET_0.DLL = f_Khz * 5 / (1152 * 8);
    UART0->OFFSET_0.DLL = uart_dll;
    UART0->OFFSET_4.DLH = 0;
    UART0->LCR &= ~BIT7;

    //Set Enable FIFO Receive Threshold 1/2 Send Threshold 1/4
    UART0->OFFSET_8.FCR = (2 << 6) | (2 << 4) | BIT2 | BIT1 | BIT0;
    
    //Set UART Parameters 
    //| Stop Bit 1 | Data Bit 8 | Odd Parity Check |
    UART0->LCR = LCR_DATA8_STOP1;

#if (UART_INT_MODE)
    //Conkfig interrupt
    UART0->OFFSET_4.IER = en;

    //Enable int8 UART0 interrupt.
    NVIC->ISER[0] = BIT8;
#endif
}

#if (UART_INT_MODE)

int32_t uart_IsSendFinish(void)
{
    return ((UART0->LSR & BIT6) && 0 == RNG_BUF_LEN(uart_SendBuf));
}

void uart_RecvFlush(void)
{
    //disable recv interrupt.
    UART0->OFFSET_4.IER &= ~RECV_INT;
    //Reset recv FIFO
    UART0->OFFSET_8.FCR |= BIT1;
    //Reset driver RNG buffer
    uart_RecvBuf->head = 0;
    uart_RecvBuf->tail = 1;
    uart_RecvBuf->err_stat = 0;
    uart_RecvBuf->buf_size = UART_RECV_BUF_SIZE;;

    UART0->OFFSET_4.IER |= RECV_INT;
}

int32_t uart_SendChar(uint8_t ch)
{
    if (NULL == uart_SendBuf || !(RNG_BUF_LEN(uart_SendBuf) < uart_SendBuf->buf_size))
    {
        return -1;
    }

    uart_SendBuf->buf[uart_SendBuf->tail] = ch;
    uart_SendBuf->tail = RNG_BUF_NEXT_TAIL(uart_SendBuf);
    
    UART0->OFFSET_4.IER |= SEND_INT;

    return 0;
}

int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len)
{
    uint32_t i;

    if (NULL == uart_SendBuf)
    {
        return -1;
    }

    for (i = 0; i < len; i++)
    {
        if (!(RNG_BUF_LEN(uart_SendBuf) < uart_SendBuf->buf_size))
        {
            break;
        }
        uart_SendBuf->buf[uart_SendBuf->tail] = pBuf[i];
        uart_SendBuf->tail = RNG_BUF_NEXT_TAIL(uart_SendBuf);
    }
    UART0->OFFSET_4.IER |= SEND_INT;

    return i;
}


int32_t uart_RecvChar(void)
{
    if (NULL == uart_RecvBuf)
    {
        return -2;
    }
    if (RNG_BUF_LEN(uart_RecvBuf) > 0)
    {
        uart_RecvBuf->head = RNG_BUF_NEXT_HEAD(uart_RecvBuf);
        return uart_RecvBuf->buf[uart_RecvBuf->head];
    }

    return -1;
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
    static uint32_t err_count = 0;
    //UART0->OFFSET_0.THR = 0xAA;
    

    switch(int_Flag)
    {
    //Receive data available
    case RECV_DATA:
    //Recv data available but not reach the recv threshold 
    case CHAR_TIMEOUT:
        for (i = 0; i < 16 && UART0->RFL != 0; i++)
        {
            if (RNG_BUF_LEN(uart_RecvBuf) < uart_RecvBuf->buf_size)
            {
                uart_RecvBuf->buf[uart_RecvBuf->tail] = UART0->OFFSET_0.RBR;
                uart_RecvBuf->tail = RNG_BUF_NEXT_TAIL(uart_RecvBuf);
            }
        }
        break;
    case TRANS_EMPTY:
        for (i = 0; i < 16 && UART0->TFL < 16; i++)
        {
            if (0 == RNG_BUF_LEN(uart_SendBuf))
            {
                UART0->OFFSET_4.IER &= ~SEND_INT;
                break;
            }
            uart_SendBuf->head = RNG_BUF_NEXT_HEAD(uart_SendBuf);
            UART0->OFFSET_0.THR = uart_SendBuf->buf[uart_SendBuf->head];
        }
        break;
    case NONE:
        break;
    case RECV_STATUS:
        //Error recv
    default:
        //Unknown interrupt occur.Disable recv interrupts.
        //UART0->OFFSET_4.IER &= ~RECV_INT;
        if (++err_count < 20)
        {
            uart_Resume();
        }
        else
        {
            UART0->OFFSET_4.IER = 0;
            NVIC->ICER[0] = BIT8;
        }
        break;
    }
    
}
#else

int32_t uart_SendChar(uint8_t ch)
{
    ;
}

int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len)
{
    ;
}

int32_t uart_RecvChar(void)
{
    ;
}
#endif
