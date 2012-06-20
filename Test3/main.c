#include "../Test2/lpc1343.h"
#include "lpc13xx.h"

int main()
{
	unsigned char toSend = 67;
	
	LPC_IOCON->PIO0_1 = 0xD8;
	LPC_GPIO0->DIR |= 0x2;
	
	while(1)
	{
			while(!(LPC_UART->LSR & (1<<6)));		
			LPC_UART->THR = toSend;
			
			LPC_GPIO0->DATA ^= 0x2;
	}
}

void pllSetup()
{
	int i;
	
	//power up system oscillator
	LPC_SYSCON->PDRUNCFG &= ~(SCB_PDRUNCFG_SYSOSC_MASK);	
	
	//setup the crystal input
	LPC_SYSCON->SYSOSCCTRL = (SCB_SYSOSCCTRL_BYPASS_DISABLED | SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ);
	
	for (i = 0; i < 200; i++)	 __nop;

  // Configure PLL
  LPC_SYSCON->SYSPLLCLKSEL = SCB_CLKSEL_SOURCE_MAINOSC;  // Select external crystal as PLL clock source
  LPC_SYSCON->SYSPLLCLKUEN = SCB_PLLCLKUEN_UPDATE;       // Update clock source
  LPC_SYSCON->SYSPLLCLKUEN = SCB_PLLCLKUEN_DISABLE;      // Toggle update register once
  LPC_SYSCON->SYSPLLCLKUEN = SCB_PLLCLKUEN_UPDATE;       // Update clock source again
	
	// Wait until the clock is updated
	while (!(LPC_SYSCON->SYSPLLCLKUEN & SCB_PLLCLKUEN_UPDATE));
	
	LPC_SYSCON->SYSPLLCTRL = (SCB_PLLCTRL_MULT_6 | (1 << SCB_PLLCTRL_DIV_BIT));
	
	  // Enable system PLL
  LPC_SYSCON->PDRUNCFG &= ~(SCB_PDRUNCFG_SYSPLL_MASK);

  // Wait for PLL to lock
  while (!(LPC_SYSCON->SYSPLLSTAT & SCB_PLLSTAT_LOCK));

  // Setup main clock (use PLL output)
  LPC_SYSCON->MAINCLKSEL = SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT;
  LPC_SYSCON->MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;     // Update clock source
  LPC_SYSCON->MAINCLKUEN = SCB_MAINCLKUEN_DISABLE;    // Toggle update register once
  LPC_SYSCON->MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;

  // Wait until the clock is updated
  while (!(LPC_SYSCON->MAINCLKUEN & SCB_MAINCLKUEN_UPDATE));

  // Disable USB clock by default (enabled in USB code)
  LPC_SYSCON->PDRUNCFG |= (SCB_PDSLEEPCFG_USBPAD_PD); // Power-down USB PHY
  LPC_SYSCON->PDRUNCFG |= (SCB_PDSLEEPCFG_USBPLL_PD); // Power-down USB PLL

  // Set system AHB clock
  LPC_SYSCON->SYSAHBCLKDIV = SCB_SYSAHBCLKDIV_DIV1;

  // Enabled IOCON clock for I/O related peripherals
  LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_IOCON;
}



/* sets up for a baud rate of 115200*/
void uartSetup()
{
		unsigned int temp;
		__disable_irq();
	
		//RX - 1_6
	//TX - 1_7
	
		LPC_IOCON->PIO1_6 = 0x1;	//RX
		LPC_IOCON->PIO1_7 = 0x1;	//TX
	
		LPC_SYSCON->UARTCLKDIV = 1;	//uart clock divider
	
		LPC_UART->LCR = (		UART_U0LCR_Word_Length_Select_8Chars |
												UART_U0LCR_Stop_Bit_Select_1Bits |
												UART_U0LCR_Parity_Disabled |
												UART_U0LCR_Parity_Select_OddParity |
												UART_U0LCR_Break_Control_Disabled |
												UART_U0LCR_Divisor_Latch_Access_Enabled);
	
		//baud rate divider section
		LPC_UART->FDR = 1<<4;	//DIVADDVAL = 0 - no fractional baud rate effect
		LPC_UART->DLM = 0;
		LPC_UART->DLL = 39;
		
		  /* Set DLAB back to 0 */
		LPC_UART->LCR = (		UART_U0LCR_Word_Length_Select_8Chars |
												UART_U0LCR_Stop_Bit_Select_1Bits |
												UART_U0LCR_Parity_Disabled |
												UART_U0LCR_Parity_Select_OddParity |
												UART_U0LCR_Break_Control_Disabled |
												UART_U0LCR_Divisor_Latch_Access_Disabled);
		
		LPC_UART->FCR = (		UART_U0FCR_FIFO_Enabled | 
												UART_U0FCR_Rx_FIFO_Reset | 
												UART_U0FCR_Tx_FIFO_Reset); 
												
		temp = LPC_UART->LSR;	//read to clear the line status
		
		__enable_irq();
		LPC_UART->IER = 		UART_U0IER_RBR_Interrupt_Enabled;
}

void SystemInit()
{
	SCB_SYSAHBCLKCTRL |= 	(	SCB_SYSAHBCLKCTRL_GPIO 		//gpio gets a clock
												| SCB_SYSAHBCLKCTRL_IOCON  //iocon gets a clock
												| SCB_SYSAHBCLKCTRL_UART		//uart gets a clock
												);	
	pllSetup();
	uartSetup();
	
}

