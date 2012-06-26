#include "Delay_Timer.h"
#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions

void DelayTimerInit()
{
SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR16_0; //timer 16_0 gets a clock
LPC_TMR16B0->PR = 72-1; //divide 72Mhz by ((72-1)+1) -> 1Mhz -> 1us/tick
LPC_TMR16B0->TCR = 0x1; //enable counting
}

/* delay by us microseconds.  Precision is on order of 1us*/
void delay(unsigned short us)
{
unsigned short lastTime = (unsigned short)LPC_TMR16B0->TC;
while((unsigned short)(LPC_TMR16B0->TC - lastTime) < us);
}
