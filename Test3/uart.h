
#define UART_RECEIVE_BUF_LENGTH			4
#define UART_SEND_BUF_LENGTH				10

//union for receiving, each incoming command is 4 bytes for now
#if UART_RECEIVE_BUF_LENGTH != 4
#error Uart receive buf length must be 4, or change the union below and this message
#endif
union{
	unsigned char raw[UART_RECEIVE_BUF_LENGTH];
	unsigned int cmd;
} uartReceiveBuffer;
unsigned int uartReceiveBufferIndex;

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
		uartReceiveBuffer.raw[uartReceiveBufferIndex++] = LPC_UART->RBR;
		if(uartReceiveBufferIndex == UART_RECEIVE_BUF_LENGTH)
		{
			//process a complete 'command'
			unsigned int command = uartReceiveBuffer.cmd;
			
			//reset the receive buffer index to receive another command
			uartReceiveBufferIndex = 0;
		}
		break;
		
	}
}

void uart_write(unsigned char b)
{
	//if empty, just send the byte right away, because there won't be a THR interrupt coming
	if(uartSendBufferEmpty)
	{
		uartSendBufferEmpty = 0;
		LPC_UART->THR = b;
	}
	else
	{
		//wait for the buffer to not be full
		while(((uartSendBufferIn+1) % UART_SEND_BUF_LENGTH) == uartSendBufferOut);
		
		uartSendBuffer[uartSendBufferIn] = b;
		uartSendBufferIn = (++uartSendBufferIn) % UART_SEND_BUF_LENGTH;
	}
}

/* sets up for a baud rate of 115200*/
void uartSetup()
{
		unsigned int temp;
		__disable_irq();
	
		//set buffer indeces to 0
		uartReceiveBufferIndex = 0;
		uartSendBufferIn = 0;
		uartSendBufferOut = 0;
		uartSendBufferEmpty = 1;
	
		LPC_IOCON->PIO1_6 = 0x1;	//RX
		LPC_IOCON->PIO1_7 = 0x1;	//TX
	
		LPC_SYSCON->UARTCLKDIV = 1;	//uart clock divider
	
		LPC_UART->LCR = (		UART_LCR_Word_Length_Select_8Chars |
												UART_LCR_Stop_Bit_Select_1Bits |
												UART_LCR_Parity_Disabled |
												UART_LCR_Parity_Select_OddParity |
												UART_LCR_Break_Control_Disabled |
												UART_LCR_Divisor_Latch_Access_Enabled);
	
		//baud rate divider section
		LPC_UART->FDR = 1<<4;	//DIVADDVAL = 0 - no fractional baud rate effect
		LPC_UART->DLM = 0;
		LPC_UART->DLL = 39;
		
		  /* Set DLAB back to 0 */
		LPC_UART->LCR = (		UART_LCR_Word_Length_Select_8Chars |
												UART_LCR_Stop_Bit_Select_1Bits |
												UART_LCR_Parity_Disabled |
												UART_LCR_Parity_Select_OddParity |
												UART_LCR_Break_Control_Disabled |
												UART_LCR_Divisor_Latch_Access_Disabled);
		
		LPC_UART->FCR = (		UART_FCR_FIFO_Enabled | 
												UART_FCR_Rx_FIFO_Reset | 
												UART_FCR_Tx_FIFO_Reset); 
												
		temp = LPC_UART->LSR;	//read to clear the line status
		
		/* Ensure a clean start, no data in either TX or RX FIFO. */
		while (( LPC_UART->LSR & (UART_LSR_THRE|UART_LSR_TEMT)) != (UART_LSR_THRE|UART_LSR_TEMT) );
		while ( LPC_UART->LSR & UART_LSR_RDR_DATA )
		{
			temp = LPC_UART->RBR;	//read data out of RX buffer
		}
		
		LPC_UART->IER = 		UART_IER_RBR_Interrupt_Enabled
											| UART_IER_THRE_Interrupt_Enabled;
		
		__enable_irq();
	
		NVIC_SetPriority(UART_IRQn, 0x10);	//high-ish priority
		NVIC_EnableIRQ(UART_IRQn);					//enable interrupts on uart
		
}

