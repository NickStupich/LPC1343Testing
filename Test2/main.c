#include "lpc13xx.h"
#include "lpc1343.h"	//my own definitions
#include "SPI_Protocol.h"

int main()
{
	InitSPI();
	while(1);
}


void SystemInit()
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

void PIOINT1_IRQHandler(void)
{
	unsigned bitPos = 5;
	unsigned int bitIntStatus = LPC_GPIO1->MIS & (1 << bitPos);	//get status of <bitPos> interrupt
	
	if(bitIntStatus)
		LPC_GPIO1->IC |= 1<<bitPos;	//clear the interrupt	
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


	
	