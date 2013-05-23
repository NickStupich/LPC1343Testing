#include "handControl.h"


int output0Threshold;
int output1Threshold;

unsigned char adjustState = 1;
unsigned short lastAdcVal;

void handControlInit(void)
{
	//set up output pins that talk to arduino	
	SET_GPIO_AS_OUTPUT(OUTPUT_0_PORT, OUTPUT_0_PIN);
	GPIO_OUTPUT(OUTPUT_0_PORT, OUTPUT_0_PIN, LOW);
	
	SET_GPIO_AS_OUTPUT(OUTPUT_1_PORT, OUTPUT_1_PIN);
	GPIO_OUTPUT(OUTPUT_1_PORT, OUTPUT_1_PIN, LOW);
	
	SET_GPIO_AS_OUTPUT(ADJUST_LED_PORT, ADJUST_LED_PIN);
	
	//random initial thresholds
	output0Threshold = 1000;
	output1Threshold = 2000;
	
	//set up the adjustment switching gpio input
	
	LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_GPIO;	//enable GPIO clock
	__disable_irq();
	
	NVIC_EnableIRQ(GPIO_IRQNUM(ADJUST_TOGGLE_PORT));
	//NVIC_SetPriority(GPIO_IRQNUM(DRDY_PORT), INTERRUPT_PRI_DRDY);
	
	LPC_GPIO(ADJUST_TOGGLE_PORT)->DIR &= ~(1<<ADJUST_TOGGLE_PIN);						//input
	LPC_GPIO(ADJUST_TOGGLE_PORT)->IS  &= ~(1<<ADJUST_TOGGLE_PIN);						//edge sensitive
	LPC_GPIO(ADJUST_TOGGLE_PORT)->IBE &= ~(1<<ADJUST_TOGGLE_PIN);						//single edge
	LPC_GPIO(ADJUST_TOGGLE_PORT)->IEV &= ~(1<<ADJUST_TOGGLE_PIN);						//falling edge sensitive
	
	LPC_GPIO(ADJUST_ENABLE_PORT)->DIR &= ~(1<<ADJUST_ENABLE_PIN);						//input
	
	//un-mask interrupt.  Assumes only 1 pin on this port will have an interrupt enabled
	LPC_GPIO(ADJUST_TOGGLE_PORT)->IE = (1<<ADJUST_TOGGLE_PIN);							
	
	__enable_irq();	
	outputAdjustState();
	
	AddEvent(EVENT_PROCESS_UART_CMD, (UART_CMD_START_HAND << 24) | (((1<<OUTPUT_0_CHANNEL) | (1<<OUTPUT_1_CHANNEL))<<16));
	
	SetupADC();
	
}


void PIOINT1_IRQHandler(void)									
{		
	adjustState ^= 1;
	outputAdjustState();
	LPC_GPIO(ADJUST_TOGGLE_PORT)->IC = 0xFF;	/*clear all interrupts on the port*/
}

void outputAdjustState(void)
{
	if(adjustState == 0)
	{
		GPIO_OUTPUT(ADJUST_LED_PORT, ADJUST_LED_PIN, LOW);
	}
	else
	{
		GPIO_OUTPUT(ADJUST_LED_PORT, ADJUST_LED_PIN, HIGH);
	}
}

void SetupADC(void)
{
  /* configure PIN GPIO0.11 for AD0 */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1UL <<  6) |   /* enable clock for GPIO      */
                                (1UL << 16) );  /* enable clock for IOCON     */

  LPC_IOCON->R_PIO0_11  =  (2UL <<  0);         /* P0.11 is AD0               */

  LPC_GPIO0->DIR &= ~(1UL << 11);               /* configure GPIO as input    */

  /* configure ADC */
  LPC_SYSCON->PDRUNCFG      &= ~(1UL <<  4);    /* Enable power to ADC block  */
  LPC_SYSCON->SYSAHBCLKCTRL |=  (1UL << 13);    /* Enable clock to ADC block  */

	//hardware scan mode - continuous interrupts at high frequency (~100kHz i think
		//faster than we need/want for sure
  /*LPC_ADC->CR          =  ( 1UL <<  0) |        // select AD0 pin             
                          ( 1UL << 21) |         // enable ADC                 
													(0xFF << 8) | 			//clock divider
													(1UL << 16);			//hardware scan mode - continuous interrupts
	*/
	
	//software controlled - write LPC_ADC->CR |= (1<<24) to trigger an interrupt.
	//this is being done in the fft loop for ~30Hz of adc reads
  LPC_ADC->CR          =  ( 1UL <<  0) |        /* select AD0 pin             */
                          (23UL <<  8) |        /* ADC clock is 24MHz/24      */
                          ( 1UL << 21);         /* enable ADC                 */ 
													
  LPC_ADC->INTEN       =  ( 1UL <<  8);         /* global enable interrupt    */

  NVIC_EnableIRQ(ADC_IRQn);                     /* enable ADC Interrupt       */
}




	unsigned short enabledPinStatus;
int diff;
void ADC_IRQHandler(void) 
{
	
	volatile unsigned short adcVal;
	unsigned int ADC_reg;

	ADC_reg = LPC_ADC->STAT;              /* Read ADC status clears interrupt   */

	ADC_reg = LPC_ADC->GDR;               /* Read conversion result             */
	adcVal = (ADC_reg >> 6) & 0x3FF;     /* Store converted value              */
	
	//whether or not button PIO1_5 is pushed
	enabledPinStatus = !((LPC_GPIO(ADJUST_ENABLE_PORT)->DATA >> ADJUST_ENABLE_PIN) & 1);
	
	if(enabledPinStatus)
	{		
		diff = adcVal - lastAdcVal;
		if(adjustState)
		{
			output0Threshold += diff * OUTPUT_0_ADC_SCALE;
			if(output0Threshold < 0)
				output0Threshold = 0;
		}
		else
		{
			output1Threshold += diff * OUTPUT_1_ADC_SCALE;
			if(output1Threshold < 0)
				output1Threshold = 0;
		}
	}
	
	lastAdcVal = adcVal;
}
