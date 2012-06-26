#include "ads_spi.h"
#include "coreFunctions.h"
#include "lpc13xx.h"
#include "uart.h"

// divide integers by two, so they don't overflow the integer based fft
#define SCALE_INTEGER(x)			(x>>1)

//struct and union to read data from the ads and convert to useful
#pragma pack(1)
typedef struct{
	unsigned char bytes[4];
} channelBytes;

typedef union{
	int value;
	channelBytes raw;
} channelUnion;
#pragma pack()

enum RunMode _runMode;

void initSpiWithAds(enum RunMode runMode)
{
	enum AdsSampleRates sampleRate = ADS_SPS_FFT;
	_runMode = runMode;
	
	if(runMode == RUN_MODE_TIME_DOMAIN)
		sampleRate = ADS_SPS_TIME;
	else if(runMode == RUN_MODE_TIME_DOMAIN)
		sampleRate = ADS_SPS_FFT;
}

void startSpiWithAds(void)
{
}

void stopSpiWithAds(void)
{
}

void initDRDYInterrupt(void)
{
	enum IRQn irqNum;
	unsigned short interruptPriority;
	LPC_GPIO_TypeDef* lpc_gpio;
	
	__disable_irq();
	
	#if DATA_READY_WIRE_PORT == 2 && DATA_READY_WIRE_PIN == 10
	lpc_gpio = LPC_GPIO2;	
	irqNum = EINT2_IRQn;
	#else
	#error data ready pin interrupt stuff not set up for the provided pin/port combo
	#endif
	
	NVIC_EnableIRQ(irqNum);
	NVIC_SetPriority(irqNum, interruptPriority);
	
	lpc_gpio->DIR &= ~(1<<DATA_READY_WIRE_PIN);						//input
	lpc_gpio->IS &= ~( 1<< DATA_READY_WIRE_PIN);					//edge sensitive
	lpc_gpio->IBE &= ~(1<<DATA_READY_WIRE_PIN);						//single edge
	lpc_gpio->IEV &= ~(1<<DATA_READY_WIRE_PIN);						//falling edge sensitive
	
	lpc_gpio->IE = (1<<DATA_READY_WIRE_PIN);							//un-mask interrupt
	
	__enable_irq();
}

void PIOINT2_IRQHandler(void)									
{								
	channelUnion cu[9];
	channelUnion* cuPtr;
	/*Read data (216bits) from the ads through SPI*/
	
	//pointer incrementing is faster than just indexing the array
	cuPtr	= &cu[0];
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6; 
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	cuPtr->raw.bytes[3] = 8;
	cuPtr->raw.bytes[2] = 7;
	(*(cuPtr++)).raw.bytes[1] = 6;
	
	if(_runMode == RUN_MODE_TIME_DOMAIN)
	{
		//send away one set of bytes over bluetooth
		//send "backwards" as msb first
		//index is timeEnabledChannel+1 since first is status bits
		cuPtr	= &cu[timeEnabledChannel+1];
		uart_write(cuPtr->raw.bytes[3]);
		uart_write(cuPtr->raw.bytes[2]);
		uart_write(cuPtr->raw.bytes[1]);
	}
	else if(_runMode == RUN_MODE_FREQ_DOMAIN)
	{
			//store everything for later.  also need to do downsampling here
			dataBuffers[8][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[7][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[6][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[5][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[4][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[3][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[2][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			dataBuffers[1][dataIndex] = SCALE_INTEGER((*(--cuPtr)).value);
			
			dataIndex++;
			dataIndex %= BUFFER_LENGTH;
	}
	
	LPC_GPIO2->IC = 0xFF;	/*clear all interrupts*/
}

