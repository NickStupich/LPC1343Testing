#include "coreFunctions.h"
#include "settings.h"
#include "uart.h"
#include "string.h"
#include "fft.h"
#include "ads_spi.h"
#include "ads_gpio.h"
#include "timers.h"

#define CHANNEL_IS_ENABLED(x)		((1<<x) & fftEnabledChannels)

#define UART_CMD_CMD_MASK				0xFF
#define UART_CMD_CHANNELS_MASK	0xFF00
#define UART_CMD_CHECK_MASK			0xFF000000

#define UART_CMD_STOP						0x1
#define UART_CMD_START_FFT			0x2
#define UART_CMD_START_TIME			0x4

/* data used for fft mode to store all the data */
int dataBuffers[NUM_CHANNELS][BUFFER_LENGTH];

/* temp buffer for data about to be fft-ed*/
int timeBuffer[BUFFER_LENGTH];

/* result of fft.  Stored as complex inside realFFT128() so it's gotta be long */
int fftBuffer[BUFFER_LENGTH+2];

/* buffer index that's being filled next*/
int dataIndex;

/* bitwise or of all enabled channels. Ex channels 0, 3 = (1<<0) | (1<<3) = 0x09 */
unsigned int fftEnabledChannels;

/* integer between 0 and 7(inclusive) with the channel that is enabled to time domain running */
unsigned int timeEnabledChannel;
void ProcessUartCommand(unsigned int cmd)
{
	int i;
	unsigned int j;
	
	if(cmd & UART_CMD_CHECK_MASK)	//error, this byte should be zero
	{
		goto fail;
	}
	
	switch(cmd & UART_CMD_CMD_MASK)
	{
		case UART_CMD_STOP:	
			break;
		
		case UART_CMD_START_FFT:
			fftEnabledChannels = cmd & UART_CMD_CHANNELS_MASK;
			if(fftEnabledChannels == 0)
			{
				goto fail;
			}	
			
			//ready the gpio pin for data ready interrupts
			initGpioInterrupt(	DATA_READY_WIRE_PORT,
													DATA_READY_WIRE_PIN,
													processDataReadyFrequencyDomain);
			
			//get the ads ready to start
			initSpiWithAds(ADS_SPS_FFT);
			
			//start up the communication with the ads
			startSpiWithAds();
			
			//set up FFTTimerInit to run in 1 second, and start doing FFTs
			AsyncTimerFunctionCall(1000000, FFTTimerInit);
			
			break;
		
		case UART_CMD_START_TIME:
			j = cmd & UART_CMD_CHANNELS_MASK;
			//check is power of two (only 1 channel enabled) and > 0
			if(	(j & (j-1)) 	//(x&(x-1)) == 0 for powers of two
					|| !j)													//check if zero
			{
				goto fail;
			}
			
			//channel is valid, need channel = log2(j)
			timeEnabledChannel = 0;
			while(j>1)
			{
				j>>=1;
				timeEnabledChannel++;
			}
			
			
			//ready the gpio pin for data ready interrupts
			initGpioInterrupt(	DATA_READY_WIRE_PORT,
													DATA_READY_WIRE_PIN,
													processDataReadyTimeDomain);
			
			//get the ads to start sampling
			initSpiWithAds(ADS_SPS_TIME);
			
			break;
		
		default:
			goto fail;
	}
	
	//got here, that means great success
	for(i=0;i<4;i++) uart_write(cmd & (0xFF<<(i*8)));
	return;
	
	fail:
	//we fucked up.  send back 0xFF FF FF FF to indicate this (no error codes...for now)
	for(i=0;i<4;i++) uart_write(0xFF);
}

/*
makes a copy of the time domain data to deal with synchronicity type stuff, calculates fft, combines bins and sends them off 
*/
void ComputeAndSendTransforms()
{
	int i;
	unsigned char transformScalingValue;
	unsigned char transformBins[FFT_BIN_COUNT];
	
	int tempDataIndex;
	
	for(i=0;i<NUM_CHANNELS;i++)
	{
		if(CHANNEL_IS_ENABLED(i))
		{
			tempDataIndex = dataIndex;
			
			//make a copy of the data to process
			memcpy(timeBuffer, &dataBuffers[i][tempDataIndex], (BUFFER_LENGTH-tempDataIndex)*sizeof(int));
			memcpy(&timeBuffer[BUFFER_LENGTH-tempDataIndex], dataBuffers[i], tempDataIndex * sizeof(int));
			
			//compute the fft, also does magnitude and condenses into first BUFFER_LENGTH/2 +1 spots
			realFFT128(timeBuffer, fftBuffer);
			
			//make a copy of the bins that are deemed useful, calculate and divide out the scaling value
			combineDataToBins(fftBuffer, transformBins, &transformScalingValue);
			
			//send stuff out over bluetooth
			sendFFTData(transformBins, transformScalingValue);
		}
	}
	
	//send the control byte to indicate end of ffts
	uart_write(CONTROL_BYTE);	
}

/*
Sends data away over uart
*/
void sendFFTData(unsigned char transformBins[], unsigned char transformScalingValue)
{
	int i;
	uart_write(transformScalingValue);
	for(i=0;i<FFT_BIN_COUNT;i++)
	{
		uart_write(transformBins[i]);
	}
}

/*
The next two functions are called whenever the data ready pin goes low and the ADS has data to be read from SPI.
They need to read that data, and make some sense of it.
*/

/* Used to process data in time domain.  It just reads from spi and sends one channel over uart */
void processDataReadyTimeDomain()
{
	
}

/*
Used to process data in frequency domain.  Reads from SPI and stores data in a buffer to be processed later 
*/
void processDataReadyFrequencyDomain()
{
	
}
