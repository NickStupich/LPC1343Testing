#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions

#define SCB_PDRUNCFG_SYSOSC_MASK						0x20
#define SCB_SYSOSCCTRL_BYPASS_DISABLED			0x0
#define SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ		0x0
#define SCB_CLKSEL_SOURCE_MAINOSC						0x1
#define SCB_PLLCLKUEN_UPDATE								0x1
#define SCB_PLLCLKUEN_DISABLE								0x0
#define SCB_PLLCTRL_MULT_4									0x3
#define SCB_PLLCTRL_MULT_6									0x5
#define SCB_PLLCTRL_DIV_BIT									5
#define SCB_PDRUNCFG_SYSPLL_MASK						0x80
#define SCB_PLLSTAT_LOCK										0x1
#define SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT	0x3
#define SCB_MAINCLKUEN_UPDATE								0x1
#define SCB_MAINCLKUEN_DISABLE							0x0
#define SCB_PDSLEEPCFG_USBPAD_PD						0x400
#define SCB_PDSLEEPCFG_USBPLL_PD						0x100
#define SCB_SYSAHBCLKDIV_DIV1								0x1

int main()
{	
	while(1);
}

/*Sets the system to use the PLL, set up convert the frequency up to 72Mhz*/
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

/*Enables interrupts on GPIO pin P1_5 on the falling edge*/
void enableGPIO1_5Interrupt()
{
	unsigned int bitPos = 5;
	
	__disable_irq();
	//set up GPIO0 for external interrupts
	
	SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_GPIO);	//give gpio a clock
	SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_IOCON);	//iocon gets a clock
	
	LPC_GPIO1->DIR &= ~(1<<bitPos);						//input
	LPC_GPIO1->IS &= ~( 1<< bitPos);					//edge sensitive
	LPC_GPIO1->IBE &= ~(1<<bitPos);						//single edge
	LPC_GPIO1->IEV &= ~(1<<bitPos);						//falling edge sensitive
	LPC_GPIO1->IE |= (1<<bitPos);							//un-mask interrupt
	
	//ISER1 |= 1<<24;	//enable interrupt for GPIO 0
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_SetPriority(EINT1_IRQn, 0x1F);
	
	//IPR14 |= (0x1F << 3);//highest priority for P0_1
	LPC_IOCON->PIO1_4 = 0x10;		//pull-up, no hysterisis, GPIO function	
	
	__enable_irq();
}

void SystemInit()
{
	pllSetup();
	enableGPIO1_5Interrupt();
}

void PIOINT1_IRQHandler(void)
{
	unsigned bitPos = 5;
	unsigned int bitIntStatus = LPC_GPIO1->MIS & (1 << bitPos);	//get status of <bitPos> interrupt
	
	if(bitIntStatus)
		LPC_GPIO1->IC |= 1<<bitPos;	//clear the interrupt	
}
