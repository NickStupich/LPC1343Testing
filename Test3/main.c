#include "../Test2/lpc1343.h"
#include "lpc13xx.h"
#include "uart.h"
#include "dataConversion3.h"


unsigned int start, end, elapsed;

int main()
{
	//unsigned char toSend = 67;
	int i;

	LPC_IOCON->PIO0_1 = 0xD8;
	LPC_GPIO0->DIR |= 0x2;
	
	//SYSAHBCLKCTRL |= (1<<9); //power up timer 0
	LPC_TMR32B0->TCR = 0x1;	//enable for counting
	
	//cu.raw.bytes[0] = 0x0;
	
	for(i=0;i<9;i++) cu[i].raw.bytes[0] = 0x0;
	
	
	start	= LPC_TMR32B0->TC;
	//TESTFFT(testData, 128);
	
	testDataConversion();
	
	end = LPC_TMR32B0->TC;
	elapsed = end - start;
	
	start	= LPC_TMR32B0->TC;
	for(i=0;i<14400;i++)
	{
		uart_write(99);
		//LPC_GPIO0->DATA ^= 0x2;
	}
	end = LPC_TMR32B0->TC;
	elapsed = end - start;
	
	while(1)
	{
			//while(!(LPC_UART->LSR & UART_LSR_THRE));		
			//LPC_UART->THR = toSend;
			//uart_write(elapsed);
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


void SystemInit()
{
	SCB_SYSAHBCLKCTRL |= 	(	SCB_SYSAHBCLKCTRL_GPIO 		//gpio gets a clock
												| SCB_SYSAHBCLKCTRL_IOCON  //iocon gets a clock
												| SCB_SYSAHBCLKCTRL_UART		//uart gets a clock
												| SCB_SYSAHBCLKCTRL_TMR32_0	//timer 0 gets a clock
												);	
	pllSetup();
	uartSetup();
	
}
