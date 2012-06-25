#include "lpc13xx.h"
#include "systemClock.h"
#include "uart.h"
#include "timers.h"

int main()
{	
	//everything is started over bluetooth, so just sit here in the loop forever
	while(1);
}

void SystemInit()
{
	//set the system to use the PLL and crank the frequency up to 72Mhz (max for processor)
	pllInit();
	
	//start up the uart - bluetooth connection
	uartInit();
	
	//set up the timer used all over to wait for delays
	DelayTimerInit();
	
	//set up the timer used to schedule ffts
	FFTTimerInit();	
}
