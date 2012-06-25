#include "ads_gpio.h"
#include "lpc13xx.h"
#include "settings.h"

/* Callback functions, restrict to 1 per port to reduce latency (and since we don't need them all)*/
void (*interruptCallback0)();
void (*interruptCallback1)();
void (*interruptCallback2)();
void (*interruptCallback3)();

/* generic handler, used on all 4 interrupts
Does not consider which pin caused the interrupt, just clears everything and calls the callback function
*/
#define GPIO_Interrupt_Handler(x)			void PIOINT ## x ## _IRQHandler(void)										\
																			{																												\
																				interruptCallback ## x ##();													\
																				LPC_GPIO ##x ##->IC = 0xFF;	/*clear all interrupts*/	\
																			}																											
												
//interrupt handlers for each gpio port																			
GPIO_Interrupt_Handler(0)							
GPIO_Interrupt_Handler(1)							
GPIO_Interrupt_Handler(2)							
GPIO_Interrupt_Handler(3)

/* Sets up a gpio interrupt that calls the provided callbackFunction
Only one interrupt / port is allowed to reduce latency checking for stuff, and since we don't
need more (i think?).  
Numbering example - GPIO2_3 would be gpioPort = 2, gpioPin = 3
*/																			
void initGpioInterrupt(int gpioPort, int gpioPin, void (*callbackFunc)())
{
	enum IRQn irqNum;
	unsigned short interruptPriority;
	LPC_GPIO_TypeDef* lpc_gpio;
	
	irqNum = (enum IRQn)(EINT0_IRQn - gpioPort);
	__disable_irq();
	
	switch(gpioPort)
	{
		case 0:
			lpc_gpio = LPC_GPIO0;
			interruptCallback0 = callbackFunc;
			interruptPriority = INTERRUPT_PRI_GPIO_0;
			break;
		
		case 1:
			lpc_gpio = LPC_GPIO1;
			interruptCallback1 = callbackFunc;
			interruptPriority = INTERRUPT_PRI_GPIO_1;
			break;
		
		case 2:
			lpc_gpio = LPC_GPIO2;
			interruptCallback2 = callbackFunc;
			interruptPriority = INTERRUPT_PRI_GPIO_2;
			break;
		
		case 3:
			lpc_gpio = LPC_GPIO3;
			interruptCallback3 = callbackFunc;
			interruptPriority = INTERRUPT_PRI_GPIO_3;
			break;
		default: //default to 0
			lpc_gpio = LPC_GPIO0;
			interruptCallback0 = callbackFunc;
			interruptPriority = INTERRUPT_PRI_GPIO_0;
			break;
	}
	
	//enable the interrupt in the nested vector interrupt controller, and set priority
	NVIC_EnableIRQ(irqNum);
	NVIC_SetPriority(irqNum, interruptPriority);
	
	/* this should probably be generalized, but for now it doesn't need to be*/
	lpc_gpio->DIR &= ~(1<<gpioPin);						//input
	lpc_gpio->IS &= ~( 1<< gpioPin);					//edge sensitive
	lpc_gpio->IBE &= ~(1<<gpioPin);						//single edge
	lpc_gpio->IEV &= ~(1<<gpioPin);						//falling edge sensitive
	
	/* this next line is kinda debateable, but for now i'm going with only 1 pin enabled / port.  Avoid some logic in the callback (which 
	is likely to be happening very often
	alternative would be 
	... |= (1<<gioPin) - would not disable other interrupts
	*/
	lpc_gpio->IE = (1<<gpioPin);							//un-mask interrupt

	__enable_irq();
	
}
