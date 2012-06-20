int main()
{

while(1);	
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


void uartSetup()
{
		__disable_irq();
	
		//RX - 1_6
	//TX - 1_7
	
		LPC_IOCON->PIO1_6 = 
		LPC_IOCON->PIO1_7 = 
		
		__enable_irq();
}

void SystemInit()
{
	pllSetup();
	uartSetup();
	
}

