#include "uart.h"
#include "lpc13xx.h"
#include "lpc1343Constants.h"
#include "coreFunctions.h"
#include "settings.h"
#include "events.h"

#define UART_RECEIVE_BUF_LENGTH			UART_CMD_LENGTH
#define UART_SEND_BUF_LENGTH				(1 + NUM_CHANNELS * (FFT_BIN_COUNT+1))

//union for receiving, each incoming command is 4 bytes for now
#if UART_RECEIVE_BUF_LENGTH != 4
#error Uart receive buf length must be 4, or change the union below and this message
#endif

union{
	unsigned char raw[UART_RECEIVE_BUF_LENGTH];
	unsigned int cmd;
} uartReceiveBuffer;
int uartReceiveBufferIndex;

//circular buffer for sending
unsigned char uartSendBuffer[UART_SEND_BUF_LENGTH];
unsigned int uartSendBufferIn;
unsigned int uartSendBufferOut;
unsigned int uartSendBufferEmpty = 1;


void UART_IRQHandler()
{
	unsigned char iirValue;
	//unsigned char temp;
	
	iirValue = LPC_UART->IIR;	//get the interrupt indentification
	
	switch(iirValue & UART_IntId_MASK)
	{
		
	case UART_IntId_THRE:		//transmit data ready
		if(uartSendBufferIn != uartSendBufferOut)
		{
			//theres something in the queue to send
			LPC_UART->THR = uartSendBuffer[uartSendBufferOut];
			
			//increment the sending buffer
			uartSendBufferOut = (++uartSendBufferOut) % UART_SEND_BUF_LENGTH;
			uartSendBufferEmpty = 0;
		}
		else
		{
			uartSendBufferEmpty = 1;
		}
		break;
		
	case UART_IntId_RDA:		//receive data available
	
		//reading from rbr clears the interrupt
		uartReceiveBuffer.raw[uartReceiveBufferIndex--] = LPC_UART->RBR;	//store backwards since little endian
		if(uartReceiveBufferIndex < 0)
		{
			//have a full command of data, process it (on another 'thread')
			AddEvent(EVENT_PROCESS_UART_CMD, uartReceiveBuffer.cmd);
			
			//reset the receive buffer index to receive another command
			uartReceiveBufferIndex = UART_RECEIVE_BUF_LENGTH-1;
		}
		break;
		
	}
}

void uart_write(unsigned char b)
{
	//if empty, just send the byte right away, because there won't be a THR interrupt coming
	if(uartSendBufferEmpty)
	{
		uartSendBufferEmpty = 0;	//no longer empty - we're going to be getting an THR empty interrupt coming
		LPC_UART->THR = b;				//send the byte
	}
	else
	{
		//wait for the buffer to not be full
		while(((uartSendBufferIn+1) % UART_SEND_BUF_LENGTH) == uartSendBufferOut);
		
		uartSendBuffer[uartSendBufferIn] = b;		//save the byte in the buffer
		uartSendBufferIn = (++uartSendBufferIn) % UART_SEND_BUF_LENGTH;			//increment the spot for the next buffer, loop around if necessary
	}
}

/* sets up for a baud rate of 115200*/
void uartInit(void)
{
		//unsigned int temp;
	
		__disable_irq();
	
		//enable clock to the uart peripheral
		LPC_SYSCON->SYSAHBCLKCTRL |=	SCB_SYSAHBCLKCTRL_UART;
	
		//set buffer indeces
		uartReceiveBufferIndex = UART_RECEIVE_BUF_LENGTH-1;
		uartSendBufferIn = 0;
		uartSendBufferOut = 0;
		uartSendBufferEmpty = 1;
	
		//set rx and tx pins as being used for uart
		LPC_IOCON_PIO(RX_PORT, RX_PIN) = IOCON_FUNC_1;
		LPC_IOCON_PIO(TX_PORT, TX_PIN) = IOCON_FUNC_1;
		
		LPC_SYSCON->UARTCLKDIV = 1;	//uart clock divider to fastest uart clock
	
		LPC_UART->LCR = (		UART_LCR_Word_Length_Select_8Chars |
												UART_LCR_Stop_Bit_Select_1Bits |
												UART_LCR_Parity_Disabled |
												UART_LCR_Parity_Select_OddParity |
												UART_LCR_Break_Control_Disabled |
												UART_LCR_Divisor_Latch_Access_Enabled);
	
		//baud rate divider section
		LPC_UART->FDR = 1<<4;
		LPC_UART->DLM = 0;
		LPC_UART->DLL = 39;
		
		  /* Set DLAB back to 0 */
		LPC_UART->LCR &= ~UART_LCR_Divisor_Latch_Access_Enabled;
		
		/* enable and reset the fifo*/
		LPC_UART->FCR = (		UART_FCR_FIFO_Enabled | 
												UART_FCR_Rx_FIFO_Reset | 
												UART_FCR_Tx_FIFO_Reset); 
												
		//read to clear the line status
		LPC_UART->LSR;
		
		/* Ensure a clean start, no data in either TX or RX FIFO. */
		while (( LPC_UART->LSR & (UART_LSR_THRE|UART_LSR_TEMT)) != (UART_LSR_THRE|UART_LSR_TEMT) );
		while ( LPC_UART->LSR & UART_LSR_RDR_DATA )
		{
			LPC_UART->RBR;//read data out of RX buffer
		}
		
		//enable interrupts on received data and transmit available
		LPC_UART->IER = 		UART_IER_RBR_Interrupt_Enabled
											| UART_IER_THRE_Interrupt_Enabled;
			
		NVIC_SetPriority(UART_IRQn, INTERRUPT_PRI_UART);	//high-ish priority
		NVIC_EnableIRQ(UART_IRQn);					//enable interrupts on uart		
		
		__enable_irq();
}
