#include "lpc13xx.h"
#include "../Test2/lpc1343.h"

#define FFT_FREQUENCY			30

/* delay by us microseconds.  Precision is on order of 1us*/
void delay(unsigned short us)
{
		unsigned short lastTime = (unsigned short)LPC_TMR16B0->TC;
		while((unsigned short)(LPC_TMR16B0->TC - lastTime) < us);
}

unsigned int start, end, elapsed;
int main()
{
	LPC_IOCON->PIO0_1 = 0xD8;
	LPC_GPIO0->DIR |= 0x2;
			
	start	= LPC_TMR32B0->TC;
	
	delay(1000);
	
	end = LPC_TMR32B0->TC;
	elapsed = end - start;
	
	while(1)
	{
			delay(50000);
			LPC_GPIO0->DATA ^= 0x2;
	}
}

void DelayTimerInit()
{
	SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR16_0;	//timer 16_0 gets a clock
	LPC_TMR16B0->PR = 72-1;	//divide 72Mhz by ((72-1)+1) -> 1Mhz -> 1us/tick
	LPC_TMR16B0->TCR = 0x1;	//enable counting
}

void FFTTimerInit()
{
	__disable_irq();
	
	SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR16_1;	//timer 16_1 gets a clock
	LPC_TMR16B1->PR = (72<<2)-1;	//divide 72Mhz by ((288-1)+1) -> 0.25Mhz -> 4us/tick
	LPC_TMR16B1->TCR = 0x0;	//disable counting (until enabled in StartFFTTimer()
	LPC_TMR16B1->MCR = 	(	
												(1<<0) 	//enable interrupt on match 0
											| (1<<1)	//reset on match 0
											);
	
	LPC_TMR16B1->MR0 = 250000 / FFT_FREQUENCY;
	
	NVIC_EnableIRQ(TIMER_16_1_IRQn);					//enable tmr16_1 interrupts
	NVIC_SetPriority(TIMER_16_1_IRQn, 0x0A);	//priority set to lowish
	
	__enable_irq();
}

void StartFFTTimer()
{
	LPC_TMR16B1->TCR = 0x1;	//enable timer counting
}

void StopFFTTimer()
{
	LPC_TMR16B1->TCR = 0x0;	//disable timer counting
}

void TIMER16_1_IRQHandler()
{
	//reset match 0 interrupt - other match/capture interrupts are disabled
	LPC_TMR16B1->IR = 0x1;
	
	
	//flip output for debugging
	//LPC_GPIO0->DATA ^= 0x2;
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
	pllSetup();
	DelayTimerInit();
	FFTTimerInit();
	
	SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR32_0;
	LPC_TMR32B0->TCR = 0x1;	//enable for counting
	
	StartFFTTimer();
}
