#include "ads_spi.h"
#include "coreFunctions.h"
#include "lpc13xx.h"
#include "uart.h"
#include "timers.h"
#include "lpc1343Constants.h"
#include "downSampling.h"
#include "ads1298Constants.h"

// divide integers by two, so they don't overflow the integer based fft
#define SCALE_INTEGER(x)			(x>>1)

// delays required by the ads between spi talkings
#define RESET_DELAY_US		(4000)
#define SPI_WRITE_DELAY   (400)
#define ADS_START_DELAY   (800)

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

//used in the DRDY Pin interrupt
channelUnion cu[9];
channelUnion* cuPtr;

#define SPI_READ_TO_PTR(ptr, index)			LPC_SSP0->DR = 0xFF;		\
																					while((LPC_SSP0->SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY );	\
																					ptr->raw.bytes[index] = LPC_SSP0->DR;

//marginally faster (1.5 us on a ~80us operation, but doesn't seem worth it due to the volatility it introduces. Maybe later...
/*int j;
#define SPI_READ_TO_PTR(ptr, index)				LPC_SSP0->DR = 0xFF;		\
																					for(j=0;j<18;j++){__nop();}	\
																					ptr->raw.bytes[index] = LPC_SSP0->DR;
*/
#define SPI_READ3_PTR_INCREMENT(ptr)			SPI_READ_TO_PTR(ptr, 3)	\
																					SPI_READ_TO_PTR(ptr, 2)	\
																					SPI_READ_TO_PTR((ptr++), 1);

#define NUM_REGISTERS_TO_RW								0x19
#define RW_REGISTERS_START_ADDR						0x0
void initSpiWithAds(enum RunMode runMode)
{
	int a;
	#if SEND_REGISTERS_OVER_UART == 1
		unsigned char datain[26];			//register information
	#endif
	
	enum AdsSampleRates sampleRate;
	unsigned char write_Array[28] = {	ADS_CMD_WREG | RW_REGISTERS_START_ADDR,		//write opcode
																		NUM_REGISTERS_TO_RW, 	//write 0x19 registers (26)
																		0x7F, 	//id (can be anything)
																		0x86, 	//config1
																		0x10, 	//config2
																		0xDC, 	//config3
																		0x03, 	//loff
																		0x10, 	//ch1set
																		0x10, 	//ch2set...
																		0x10, 
																		0x10,
																		0x10, 
																		0x10, 
																		0x10, 
																		0x10, 
																		0, 			//rld_sensp
																		0, 			//rld_sensn
																		0xFF, 	//loff_sensp	
																		0x02, 	//loff_sensn
																		0, 			//loff_flip
																		0xFE, 	//loff_statp
																		0x06,		//loff_statn
																		0, 			//gpio
																		0,			//pace
																		0, 			//resp
																		0x02, 	//config4
																		0x0A, 	//wct1
																		0xE3		//wct2
		};

	//set the channelUnion values to 0 for 3rd byte of int
	for(a=0;a<9;a++)
	{
		cu[a].raw.bytes[0] = 0;
	}
	
	InitSPI();
		
	sampleRate = ADS_SPS_FFT;
	_runMode = runMode;
	
	if(runMode == RUN_MODE_TIME_DOMAIN)
		sampleRate = ADS_SPS_TIME;
	else if(runMode == RUN_MODE_FREQ_DOMAIN)
		sampleRate = ADS_SPS_FFT;

	//set the sampling frequency
	write_Array[3] = 0x80 | sampleRate;	
	
	//Configure ADS Start pin
	LPC_IOCON_PIO(START_PORT, START_PIN) = IOCON_FUNC_GPIO | IOCON_MODE_PULL_UP;
	SET_GPIO_AS_OUTPUT(START_PORT, START_PIN);
	
	//Configure ADS Reset pin
	LPC_IOCON_PIO(RESET_PORT, RESET_PIN) = IOCON_FUNC_GPIO | IOCON_MODE_PULL_UP;
	SET_GPIO_AS_OUTPUT(RESET_PORT, RESET_PIN);
		
	//Start low, Do not begin conversions
	GPIO_OUTPUT(START_PORT, START_PIN, LOW);
	
	//chip select high
	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);

	//Reset ADS 1298
	GPIO_OUTPUT(RESET_PORT, RESET_PIN, HIGH);
	delay(RESET_DELAY_US);
	GPIO_OUTPUT(RESET_PORT, RESET_PIN, LOW);
	delay(RESET_DELAY_US);
	GPIO_OUTPUT(RESET_PORT, RESET_PIN, HIGH);
	
	//Stop read data continuously mode
	GPIO_OUTPUT(CS_PORT, CS_PIN, LOW);
	delay(SPI_WRITE_DELAY);
	SPI0_Write(ADS_CMD_SDATAC);
	delay(SPI_WRITE_DELAY);
	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);
	delay(SPI_WRITE_DELAY);
	
	//chip select low
	GPIO_OUTPUT(CS_PORT, CS_PIN, LOW);
	
	for (a=0; a<28; a++){
		SPI0_Write(write_Array[a]);
		delay(SPI_WRITE_DELAY);
	}

	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);
	delay(SPI_WRITE_DELAY);
	
	#if SEND_REGISTERS_OVER_UART == 1
	
		//Read Registry Commands
		GPIO_OUTPUT(CS_PORT, CS_PIN, LOW);
		SPI0_Write(ADS_CMD_RREG | RW_REGISTERS_START_ADDR);
		delay(SPI_WRITE_DELAY);
		SPI0_Write(NUM_REGISTERS_TO_RW);
		delay(SPI_WRITE_DELAY);
		
		for (a=0; a<26; a++){
			datain[a] = SPI0_Read();
			uart_write(datain[a]);
		}
		
		uart_write(0);	//to make it send 27 bytes - a multiple of 3 so it can keep working after
	#endif
	
	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);
	
	//Enable Read Data Continuously Mode
	GPIO_OUTPUT(CS_PORT, CS_PIN, LOW);
	delay(SPI_WRITE_DELAY);
	SPI0_Write(ADS_CMD_RDATAC);
	delay(SPI_WRITE_DELAY);
	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);
	delay(SPI_WRITE_DELAY);
	
	//start high to begin conversions
	GPIO_OUTPUT(START_PORT, START_PIN, HIGH);
	delay(ADS_START_DELAY);
	
	return;
}


void stopSpiWithAds(void)
{
	GPIO_OUTPUT(START_PORT, START_PIN, LOW);
}

void initDRDYInterrupt(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_GPIO;	//enable GPIO clock
	
	__disable_irq();
	
	NVIC_EnableIRQ(GPIO_IRQNUM(DRDY_PORT));
	NVIC_SetPriority(GPIO_IRQNUM(DRDY_PORT), INTERRUPT_PRI_DRDY);
	
	LPC_GPIO(DRDY_PORT)->DIR &= ~(1<<DRDY_PIN);						//input
	LPC_GPIO(DRDY_PORT)->IS  &= ~(1<<DRDY_PIN);						//edge sensitive
	LPC_GPIO(DRDY_PORT)->IBE &= ~(1<<DRDY_PIN);						//single edge
	LPC_GPIO(DRDY_PORT)->IEV &= ~(1<<DRDY_PIN);						//falling edge sensitive
	
	//un-mask interrupt.  Assumes only 1 pin on this port will have an interrupt enabled
	LPC_GPIO(DRDY_PORT)->IE = (1<<DRDY_PIN);							
	
	__enable_irq();	
}



void PIOINT0_IRQHandler(void)									
{								
	int a;// ,b, data;

	/*Read data (216bits) from the ads through SPI*/
	
	//pointer incrementing is faster than just indexing the array
	cuPtr	= &cu[0];
	
	GPIO_OUTPUT(CS_PORT, CS_PIN, LOW);
	
	for(a=0; a<9; a++)
	{
		SPI_READ3_PTR_INCREMENT(cuPtr);		
	}
	
	GPIO_OUTPUT(CS_PORT, CS_PIN, HIGH);
	
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
			int downSampledData[NUM_CHANNELS];
		
			//performDownSampling returns true if the value should be counted, false if it's between samples to be saved
			if(performDownSampling(((int*)&cu)+1, downSampledData))
			{
				for(a=0;a<NUM_CHANNELS;a++)
				{
					dataBuffers[a][dataIndex] = SCALE_INTEGER(downSampledData[a]);
				}
			}
			
			dataIndex++;
			dataIndex %= BUFFER_LENGTH;
	}
	
	LPC_GPIO(DRDY_PORT)->IC = 0xFF;	/*clear all interrupts on the port*/
}

void SPI0_Write(unsigned char Data)
{
	/* Move on only if NOT busy and TX FIFO not full. */
  while ((LPC_SSP0->SR & (SSP_SSP0SR_TNF_NOTFULL | SSP_SSP0SR_BSY_BUSY)) != SSP_SSP0SR_TNF_NOTFULL);
  LPC_SSP0->DR = Data;
		
	while ( (LPC_SSP0->SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY );
  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
  is left in the FIFO. */
  LPC_SSP0->DR;
	return;
}



//read from SPI0
unsigned char SPI0_Read(void)
{
	unsigned char Data;
	//for dummy data write 0xFF to SPI to activate clock while reading the incoming data from ADS
	LPC_SSP0->DR = 0xFF;
	while((LPC_SSP0->SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY );
	Data = LPC_SSP0->DR;
	
	return Data;
}

void InitSPI()
{
	
		//Enable power to SSP0	
	LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_SPI0;
		
	//Set Peripheral Clock Frequency to 72MHz
	LPC_SYSCON->SSP0CLKDIV = 1;     //Divide by 2
	
	// Configure SPI PINs
	LPC_IOCON->SCK_LOC = (1<<0);					  //SCK0 pin location at PIO2_11
	
	LPC_IOCON_PIO(SCLK_PORT, SCLK_PIN) = IOCON_FUNC_1;		//pin mode SCLK
	LPC_IOCON_PIO(MISO_PORT, MISO_PIN) = IOCON_FUNC_1;		//pin mode MISO
	LPC_IOCON_PIO(MOSI_PORT, MOSI_PIN) = IOCON_FUNC_1;		//pin mode MOSI
	LPC_IOCON_PIO(CS_PORT, CS_PIN) = 	IOCON_FUNC_GPIO | IOCON_MODE_PULL_UP;	//gpio with pull up
	SET_GPIO_AS_OUTPUT(CS_PORT, CS_PIN);
	
	LPC_SYSCON->PRESETCTRL |= (7<<0);				//Pull SSP0 block out of reset mode
	
	LPC_SSP0->CR0 = (	SSP_CR0_DSS_8BIT			//8 bits transfer	
									| SSP_CR0_FRF_SPI				//spi setup
									| SSP_CR0_CPOL_LOW			//clock polarity low
									| SSP_CR0_CPHA_BACK			//clock out phase transition back
									);
									
	LPC_SSP0->CPSR |= 0x14;									//CPSDVSR = 2, clock prescaler
	
	LPC_SSP0->CR1 = SSP_CR1_ENABLED;				//Enable SSP Controller
	
}
