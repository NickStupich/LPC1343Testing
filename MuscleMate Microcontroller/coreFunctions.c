#include "coreFunctions.h"
#include "uart.h"
#include "string.h"
#include "fft.h"
#include "ads_spi.h"
#include "timers.h"
#include "settings.h"
#include "lpc13xx.h"
#include "lpc1343Constants.h"
#include "core_cm3.h"

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
unsigned int cmdGlobal;
void ProcessUartCommand(unsigned int cmd)
{
	int i;
	unsigned int j;
	cmdGlobal = cmd;
	stopPwdnTimer();
		
	if(UART_GET_CHECK(cmd))	//error, this byte should be zero
	{
		goto fail;
	}
	
	switch(UART_GET_CMD(cmd))
	{
		case UART_CMD_STOP:	
			if(	_runMode == RUN_MODE_TIME_DOMAIN
				|| _runMode == RUN_MODE_FREQ_DOMAIN)
			{
				//get the ads to stop sending updates
				stopAdsConversions();
				if(_runMode == RUN_MODE_FREQ_DOMAIN)
				{
					//stop performing (and sending) ffts
					StopFFTTimer();
				}
				
				uart_clearBuffer();
			}
			else
			{
				goto fail;
			}
			
			_runMode = RUN_MODE_STOPPED;
	
			startPwdnTimer();
			
			break;
		
		case UART_CMD_START_FFT:
			if(_runMode != RUN_MODE_STOPPED)
			{
				goto fail;
			}
		
			fftEnabledChannels = UART_GET_CHANNELS(cmd);
			if(fftEnabledChannels == 0)
			{
				goto fail;
			}
			
			//get the ads ready to start
			initSpiWithAds(RUN_MODE_FREQ_DOMAIN);
			
			//start receiving spi updates
			initDRDYInterrupt();
			
			//set up FFTTimerInit to run in 1 second, and start doing FFTs
			AsyncTimerFunctionCall(1000000, StartFFTTimer);
			
			_runMode = RUN_MODE_FREQ_DOMAIN;
			
			break;
		
		case UART_CMD_START_TIME:
			if(_runMode != RUN_MODE_STOPPED)
			{
				goto fail;
			}
			
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
			AsyncTimerFunctionCall(100000, initDRDYInterrupt);
			
			_runMode = RUN_MODE_TIME_DOMAIN;
			
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
	startPwdnTimer();
	//we fucked up.  send back 0xFF FF FF FF to indicate this (no error codes...for now)
	for(i=UART_CMD_LENGTH-1;i>=0;i--) uart_write(0xFF);
	for(i=UART_CMD_LENGTH-1;i>=0;i--) uart_write((cmd & (0xFF<<(i*8)))>>(i*8));
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


void ResetIntoISP()
{
	IAP iap_entry = (IAP) IAP_LOCATION;
  uint32_t command[5], result[4];
	
	//stop the ads doing stuff and interrupting things
	stopAdsConversions();
	delay(ISP_RESET_ADS_STOP_DELAY);
		
	//stop UART
  /* Disable UART interrupts */
  LPC_UART->IER = 0;
  /* Disableinterrupts in NVIC */
	NVIC->ICER[0] = 0xFFFFFFFF;

  /* Ensure a clean start, no data in either TX or RX FIFO. */
  while (( LPC_UART->LSR & (UART_LSR_THRE|UART_LSR_TEMT)) != (UART_LSR_THRE|UART_LSR_TEMT) );
  while ( LPC_UART->LSR & UART_LSR_RDR_DATA )
  {
		LPC_UART->RBR;/* Dump data from RX FIFO */
  }

  /* Read to clear the line status. */
	LPC_UART->LSR;
	
	/* make sure 32-bit Timer 1 is turned off before calling ISP */
	NVIC_DisableIRQ(TIMER_32_1_IRQn); //this line does not seem necessary
	LPC_TMR32B1->MCR = 0x0; //prevents screw ups with bluetooth communication
  /* make sure 32-bit Timer 1 is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_TMR32_1;
  /* make sure GPIO clock is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_GPIO;
  /* make sure IO configuration clock is turned on before calling ISP */
  LPC_SYSCON->SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_IOCON;
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

void pwup()
{
	SET_GPIO_AS_OUTPUT(ADS_PWDN_PORT, ADS_PWDN_PIN);
	SET_GPIO_AS_OUTPUT(BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN);
	GPIO_OUTPUT(ADS_PWDN_PORT, ADS_PWDN_PIN, HIGH);
	GPIO_OUTPUT(BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, HIGH);
}

void pwdn(unsigned char mode)
{
	//SET_GPIO_AS_OUTPUT(ADS_PWDN_PORT, ADS_PWDN_PIN); //unnecessary if pwUp used first
	//SET GPIO_AS_OUTPUT(BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN); //same as above
	GPIO_OUTPUT(ADS_PWDN_PORT, ADS_PWDN_PIN, LOW);
	GPIO_OUTPUT(BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, LOW);
	
	if(mode == SLEEP)
	{
		LPC_PMU->PCON &= ~(1<<1); //sleep enable
		SCB->SCR &= ~(1<<2); //light sleep
	}
	else
	{
		if(mode == PWDN)
		{
			LPC_PMU->PCON |= 1<<1; //power down enable
		}
		else //DEEPSLEEP
		{
			LPC_PMU->PCON &= ~(1<<1); //sleep enable
			LPC_SYSCON->PDSLEEPCFG |= 0xFFF; //turn off BOD & WDT
			//more wakeup stuff
		}
		SCB->SCR |= 1<<2; //deep (sleep or power down)
		LPC_SYSCON->PDRUNCFG &= ~(
														(1<<0) //IRC output on at wakeup
													|	(1<<1) //IRC on at wakeup
													);
	}
	__WFI();
}
