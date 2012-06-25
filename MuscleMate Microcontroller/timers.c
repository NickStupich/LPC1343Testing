#include "lpc13xx.h"
#include "lpc1343Constants.h"
#include "coreFunctions.h"
#include "settings.h"
#include "timers.h"

void TIMER16_1_IRQHandler()
{
	//calculate all the frequency domain stuff and send it off
	ComputeAndSendTransforms();
	
	//reset match 0 interrupt - other match/capture interrupts are disabled
	LPC_TMR16B1->IR = 0x1;
}

void StartFFTTimer()
{
	LPC_TMR16B1->TCR = 0x1;	//enable timer counting
}

void StopFFTTimer()
{
	LPC_TMR16B1->TCR = 0x0;	//disable timer counting
}

void FFTTimerInit()
{
	__disable_irq();
	
	LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR16_1;	//timer 16_1 gets a clock
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

void DelayTimerInit()
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR16_0;	//timer 16_0 gets a clock
	LPC_TMR16B0->PR = 72-1;	//divide 72Mhz by ((72-1)+1) -> 1Mhz -> 1us/tick
	LPC_TMR16B0->TCR = 0x1;	//enable counting
}

void delay(unsigned short us)
{
		unsigned short lastTime = (unsigned short)LPC_TMR16B0->TC;
		while((unsigned short)(LPC_TMR16B0->TC - lastTime) < us);
}

//TODO: fill the following
/* 
Sets up a timer to run once and call the provided function after the delay, specified in us
Functions returns immediately, and **should** call the function after the appropriate delay.
If theres already a function waiting, this will wait until that has been called and THEN set up the delay
So don't overlap async function call setups
*/
void AsyncTimerFunctionCall(unsigned int delay, void (*callbackFunc)())
{
	
	
}

//TODO: also fill in this
void TIMER32_0_IRQHandler()
{
	
}
