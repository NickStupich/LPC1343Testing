#ifndef _UART
#define _UART

/* last command received.  Has to be a global variable so that ProcessCommand() can be called asynchronously*/
extern unsigned int uartCommand;

void uart_write(unsigned char b);
void uartInit(void);
void uart_clearBuffer(void);
#endif
