#include "lpc13xx.h"
#include "systemClock.h"
#include "uart.h"
#include "timers.h"
#include "events.h"
#include "coreFunctions.h"
#include "settings.h"

int main()
{	
	//set the system to use the PLL and crank the frequency up to 72Mhz (max for processor)
	pllInit();
	
	//set up the timer used all over to wait for delays
	DelayTimerInit();
	
	//set up the timer used to schedule ffts
	FFTTimerInit();	
	
	//start up the uart - bluetooth connection. Do this last in case we get somethign right away
	uartInit();
	
	LPC_IOCON->PIO3_0 = 0xD8;
	LPC_GPIO3->DIR |= (1<<0);
	
	LPC_GPIO3->DATA &= ~(1<<0);
	
	while(1)
	{
		ProcessEvents();
		delay(50000);
		//LPC_GPIO3->DATA ^= (1<<0);
	}
}

/*
Don't use this.  Was causing problems - setting address of function pointers to 0 for some unknown reason 
*/
void SystemInit()
{
}
