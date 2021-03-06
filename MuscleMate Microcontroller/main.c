#include "lpc13xx.h"
#include "systemClock.h"
#include "uart.h"
#include "timers.h"
#include "events.h"
#include "coreFunctions.h"
#include "settings.h"
#include "ads_spi.h"
#include "lpc1343Constants.h"
#include "uart.h"

int main()
{		
	//set the system to use the PLL and crank the frequency up to 72Mhz (max for processor)
	pllInit();
	
	//set up the timer used all over to wait for delays
	DelayTimerInit();
	
	
	//set up the timer used to schedule ffts
	FFTTimerInit();	
	
	//set up pins for ads communication
	InitPinsForAdsCommunication();
	
	//ads boots up with 250sps data ready pin, stop this
	stopAdsConversions();
	
	//start up the uart - bluetooth connection. Do this last in case we get something right away
	uartInit();
	
	LPC_IOCON->PIO3_2 = 0xD8;
	LPC_GPIO3->DIR |= (1<<2);
	LPC_GPIO3->DATA &= ~(1<<2);
	
	//pwup();
	//pwdnTimerInit();
	//startPwdnTimer();

	while(1)
	{
		// event processing loop to deal with deadlocks if everything were to run straight from interrupts
		ProcessEvents();
		//delay(60000);
	}
}

/* Don't use this.  Was causing problems - setting address of function pointers to 0 for some unknown reason */
void SystemInit(){}
