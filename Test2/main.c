#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions
//#include "SPI_Protocol.h"

void (*callbackFunc)(void);
void dataBitFlipper(void);
void _SystemInit(void);
volatile int x;

int main()
{	
	
	LPC_IOCON->PIO2_6 = 0xD8;
	LPC_GPIO2->DIR |= (1<<6);
	
	LPC_IOCON->PIO0_7 = 0xD8;
	LPC_GPIO0->DIR |= (1<<7);
	x = 1;
	_SystemInit();
	//callbackFunc = dataBitFlipper;
	
	//InitSPI();
	while(1)
	{
		//callbackFunc();
		LPC_GPIO0->DATA ^= (1<<7);
	}
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
	
	LPC_GPIO1->DIR &= ~(1<<bitPos);						//input
	LPC_GPIO1->IS &= ~( 1<< bitPos);					//edge sensitive
	LPC_GPIO1->IBE &= ~(1<<bitPos);						//single edge
	LPC_GPIO1->IEV &= ~(1<<bitPos);						//falling edge sensitive
	LPC_GPIO1->IE |= (1<<bitPos);							//un-mask interrupt
	
	//ISER1 |= 1<<24;	//enable interrupt for GPIO 0
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_SetPriority(EINT1_IRQn, 0x1F);
	
	
	//NVIC_EnableIRQ(BusFault_IRQn);
	//NVIC_EnableIRQ(MemoryManagement_IRQn);
	
	//IPR14 |= (0x1F << 3);//highest priority for P0_1
	//LPC_IOCON->PIO1_5 = 0x10;		//pull-up, no hysterisis, GPIO function	
	LPC_GPIO1->IC = 0x0;	//clear interrupts
	
	__enable_irq();
}

void dataBitFlipper(void)
{
	LPC_GPIO2->DATA ^= (1<<6);
}

void SystemInit(){}

void _SystemInit()
{	
	//dataBitFlipper();
	//callbackFunc = (void*)((int)dataBitFlipper -1);
	callbackFunc = dataBitFlipper;
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);	//enable GPIO clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);	//Enable IOCON clock
	
	pllSetup();
	enableGPIO1_5Interrupt();	
	
	//callbackFunc = dataBitFlipper;
}

void PIOINT1_IRQHandler(void)
{
	
	/*
	if(x == 1)
	{
		//dataBitFlipper();
		LPC_GPIO2->DATA ^= (1<<6);
	}
	else
	{
		__nop();
	}
	*/
	//callbackFunc();
	dataBitFlipper();
	//LPC_GPIO2->DATA ^= (1<<6);
	/*
	unsigned bitPos = 5;
	unsigned int bitIntStatus = LPC_GPIO1->MIS & (1 << bitPos);	//get status of <bitPos> interrupt
	
	if(bitIntStatus)
		LPC_GPIO1->IC |= 1<<bitPos;	//clear the interrupt	
	*/
	//LPC_GPIO1->IC |= (1<<5);
	LPC_GPIO1->IC = 0xFF;
	//__nop();
	//__nop();
}

/*
//setup a pin for GPIO input/output. offset address from 0x000 to 0x0BC. Pin direction - 0 for input pin and 1 for output pin
void GPIOPinSetup(unsigned char offset, char direction){
	if (offset < 0) return;
	
	unsigned long IOCON_Address = LPC_IOCON_BASE + offset;
	
	//Cast the address of the pin selected to a varaible
	IOCON_Value = (*((volatile unsigned long *) IOCON_Address))
		
		
	IOCON_Value = 0xD0			//GPIO function, pull-up resistor, no hysteris 
	
}
*/

	
