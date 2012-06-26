#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions
#include "SPI_Protocol.h"
#include "Delay_Timer.h"

int main()
{

	InitSPI();
	Init_SPI_wADS();
	
	
	while(1)
	{

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



/*Enables interrupts on GPIO pin P0_5 on the falling edge*/
void enableGPIO0_5Interrupt()
{
	unsigned int bitPos = 5;
	
	__disable_irq();
	//set up GPIO0 for external interrupts
	
	LPC_GPIO0->DIR &= ~(1<<bitPos);						//input
	LPC_GPIO0->IS &= ~( 1<< bitPos);					//edge sensitive
	LPC_GPIO0->IBE &= ~(1<<bitPos);						//single edge
	LPC_GPIO0->IEV &= ~(1<<bitPos);						//falling edge sensitive
	LPC_GPIO0->IE |= (1<<bitPos);							//un-mask interrupt
	
	//ISER1 |= 1<<24;	//enable interrupt for GPIO 0
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_SetPriority(EINT0_IRQn, 0x1F);
	
	//IPR14 |= (0x1F << 3);//highest priority for P0_1
	//LPC_IOCON->PIO1_4 = 0x10;		//pull-up, no hysterisis, GPIO function	
	
	__enable_irq();
}

void SystemInit()
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);	//enable GPIO clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);	//Enable IOCON clock
	
	pllSetup();
	enableGPIO0_5Interrupt();
	DelayTimerInit();
	
}

void PIOINT0_IRQHandler(void)
{
	int a;
	int Data;
	
	
	LPC_GPIO0->DATA &= ~(1<<2); 							//Set CS pin low
	for(a=0; a<27; a++)
	{
				LPC_SSP0->DR = 0x00;
				while((LPC_SSP0->SR & 0x00000010));
				Data = LPC_SSP0->DR;
	}
	
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin high
	
	LPC_GPIO0->IC = 0xFF;	//clear the interrupt	
}
