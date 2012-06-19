#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions

int main()
{
	
	while(1);
}


void SystemInit()
{
	unsigned int bitPos = 1;
	
	__disable_irq();
	//set up GPIO0 for external interrupts
	
	SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_GPIO);	//give gpio a clock
	SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_IOCON);	//iocon gets a clock
	
	LPC_GPIO0->DIR &= ~(1<<bitPos);						//input
	LPC_GPIO0->IS &= ~( 1<< bitPos);					//edge sensitive
	LPC_GPIO0->IBE &= ~(1<<bitPos);						//single edge
	LPC_GPIO0->IEV &= ~(1<<bitPos);						//falling edge sensitive
	LPC_GPIO0->IE |= (1<<bitPos);							//un-mask interrupt
	
	//ISER1 |= 1<<24;	//enable interrupt for GPIO 0
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_SetPriority(EINT0_IRQn, 0x1F);
	
	//IPR14 |= (0x1F << 3);//highest priority for P0_1
	LPC_IOCON->PIO0_1 = 0x10;		//pull-up, no hysterisis, GPIO function
	
	
	
	__enable_irq();
}

void PIOINT0_IRQHandler(void)
{
	unsigned bitPos = 1;
	unsigned int bitIntStatus = LPC_GPIO0->MIS & (1 << bitPos);	//get status of <bitPos> interrupt
	
	if(bitIntStatus)
		LPC_GPIO0->IC |= 1<<bitPos;	//clear the interrupt	
}
