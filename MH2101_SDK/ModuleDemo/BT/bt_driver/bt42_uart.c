#include <stdio.h>
#include <stdint.h>
#include "hw_config.h"
#include "bt42_uart.h"



#define	MAX_BUFFER_SIZE			2048

typedef struct uart_param_tag
{
	volatile uint16_t wptr;
	volatile uint16_t rptr;
	volatile uint8_t flag;
	uint8_t r_buffer[MAX_BUFFER_SIZE];
}uart_param_s;

uart_param_s uart_param_t = {
	.wptr = 0,
	.rptr = 0,
	.flag = 0,
};

void host_uart_send(uint8_t *pbuff, uint16_t len)
{
	for(int i=0; i< len;i++)
	{
		while(!UART_IsTXEmpty(UART1));
		UART_SendData(UART1, pbuff[i]);
	}
}

uint16_t host_uart_read(uint8_t *pbuff, uint16_t len)
{
	uint16_t uilen =0;
	while(len--)
	{
		
		if((uart_param_t.rptr != uart_param_t.wptr) || (uart_param_t.flag == 1))
		{
			uart_param_t.flag = 0;
			*pbuff = uart_param_t.r_buffer[uart_param_t.rptr];
			uart_param_t.rptr = (uart_param_t.rptr + 1) % MAX_BUFFER_SIZE;
			pbuff ++;
			uilen ++;
		}
		else
		{
			break;
		}
		
	}
	return uilen;
}

void UART1_IRQHandler(void)
{
    UART_TypeDef * UARTx = UART1;

    switch(UARTx->OFFSET_8.IIR & 0x0f)
    {
		case UART_IT_ID_CHAR_TIMEOUT:
	    case UART_IT_ID_RX_RECVD:
	    {
				while(UART_IsRXFIFONotEmpty(UARTx))
				{
					if((uart_param_t.flag == 0) || (uart_param_t.wptr != uart_param_t.rptr)){
							uart_param_t.flag = 1;
							uart_param_t.r_buffer[uart_param_t.wptr] = UART_ReceiveData(UARTx);
							uart_param_t.wptr = (uart_param_t.wptr + 1) % MAX_BUFFER_SIZE;
					}
					
				}
	    }
	    break;
	    case UART_IT_ID_TX_EMPTY:
	    {
	        
	    }
	    break;
	    case UART_IT_ID_MODEM_STATUS:
	    {
	        uint32_t msr = UARTx->MSR;          
	    }
	    break;
	    case UART_IT_ID_LINE_STATUS:
	    {
	        uint32_t lsr = UARTx->LSR;
	        
	    }   
	    break;
	    case UART_IT_ID_BUSY_DETECT:
	    {
	        uint32_t usr = UARTx->USR;          
	    }   
	    break;
    	default:
       		break;
    }
}
