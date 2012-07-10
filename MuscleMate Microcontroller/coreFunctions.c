#include "coreFunctions.h"
#include "uart.h"
#include "string.h"
#include "fft.h"
#include "ads_spi.h"
#include "timers.h"
#include "settings.h"
#include "lpc13xx.h"
#include "lpc1343Constants.h"

#define CHANNEL_IS_ENABLED(x)		((1<<x) & fftEnabledChannels)

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

/* mode that we're rcurrently running in.  Either time or fft for now*/
enum RunMode _runMode;

void ProcessUartCommand(unsigned int cmd)
{
	int i;
	unsigned int j;
		
	if(UART_GET_CHECK(cmd))	//error, this byte should be zero
	{
		goto fail;
	}
	
	switch(UART_GET_CMD(cmd))
	{
		case UART_CMD_STOP:	
			//get the ads to stop sending updates
			stopSpiWithAds();
			
			if(_runMode == RUN_MODE_FREQ_DOMAIN)
			{
				//stop performing (and sending) ffts
				StopFFTTimer();
			}
			
			break;
		
		case UART_CMD_START_FFT:
			fftEnabledChannels = UART_GET_CHANNELS(cmd);
			if(fftEnabledChannels == 0)
			{
				goto fail;
			}
			
			initDRDYInterrupt();
			
			//get the ads ready to start
			initSpiWithAds(RUN_MODE_FREQ_DOMAIN);
			
			//set up FFTTimerInit to run in 1 second, and start doing FFTs
			AsyncTimerFunctionCall(1000000, FFTTimerInit);
			
			break;
		
		case UART_CMD_START_TIME:
			
			j = UART_GET_CHANNELS(cmd);
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
						
			//get the ads to start sampling
			initSpiWithAds(RUN_MODE_TIME_DOMAIN);
			
			//enable interrupts on the data ready pin
			//initDRDYInterrupt();
			AsyncTimerFunctionCall(100000, initDRDYInterrupt);
			
			break;
			
		case UART_CMD_ENTER_ISP:
			ResetIntoISP();
			break;
		
		default:
			goto fail;
	}
	
	
	//got here, that means great success
	for(i=UART_CMD_LENGTH-1;i>=0;i--) uart_write((cmd & (0xFF<<(i*8)))>>(i*8));
	return;
	
	fail:
	
	//we fucked up.  send back 0xFF FF FF FF to indicate this (no error codes...for now)
	for(i=UART_CMD_LENGTH-1;i>=0;i--) uart_write(0xFF);
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

typedef void (*IAP)(unsigned int[], unsigned int[]);
#define IAP_LOCATION 0x1fff1ff1
void ResetIntoISP()
{
	 IAP iap_entry = (IAP) IAP_LOCATION;
  uint32_t command[5], result[4];
		
	//stop UART
	 //uint32_t temp;
  /* Disable UART interrupts */
  LPC_UART->IER = 0;
  /* Disableinterrupts in NVIC */
	NVIC->ICER[0] = 0xFFFFFFFF;

  /* Ensure a clean start, no data in either TX or RX FIFO. */
  while (( LPC_UART->LSR & (UART_LSR_THRE|UART_LSR_TEMT)) != (UART_LSR_THRE|UART_LSR_TEMT) );
  while ( LPC_UART->LSR & UART_LSR_RDR_DATA )
  {
		//temp = LPC_UART->RBR;	/* Dump data from RX FIFO */
		LPC_UART->RBR;
  }

  /* Read to clear the line status. */
  //temp = LPC_UART->LSR;
	LPC_UART->LSR;
	
	//done stopping uart

  /* make sure 32-bit Timer 1 is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= 0x00400;
  /* make sure GPIO clock is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= 0x00040;
  /* make sure IO configuration clock is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= 0x10000;
  /* make sure AHB clock divider is 1:1 */
  LPC_SYSCON->SYSAHBCLKDIV = 1;

  /* Send Reinvoke ISP command to ISP entry point*/
  command[0] = 57;

  /* Don't? Set stack pointer to ROM value (reset default).
     This must be the last piece of code executed before calling ISP,
     because most C expressions and function returns will fail after
     the stack pointer is changed.
   */
  //__set_MSP(*((uint32_t *) 0x1FFF0000)); /* inline asm */

  /* Invoke ISP. We call "iap_entry" to invoke ISP because the ISP entry
     is done through the same command interface as IAP. */
  iap_entry(command, result);
}
