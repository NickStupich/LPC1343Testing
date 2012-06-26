#ifndef _CORE_FUNCTIONS
#define _CORE_FUNCTIONS

#include "settings.h"

/* data used for fft mode to store all the data */
extern int dataBuffers[NUM_CHANNELS][BUFFER_LENGTH];

/* temp buffer for data about to be fft-ed*/
extern int timeBuffer[BUFFER_LENGTH];

/* result of fft.  Stored as complex inside realFFT128() so it's gotta be long */
extern int fftBuffer[BUFFER_LENGTH+2];

/* buffer index that's being filled next*/
extern int dataIndex;

/* bitwise or of all enabled channels. Ex channels 0, 3 = (1<<0) | (1<<3) = 0x09 */
extern unsigned int fftEnabledChannels;

/* integer between 0 and 7(inclusive) with the channel that is enabled to time domain running */
extern unsigned int timeEnabledChannel;

/*
Processes a 4-byte command from bluetooth
breakdown:
byte 0: command:	0x1 - stop
									0x2 - start frequency domain
									0x4 - start time domain
byte 1: channels: each bit indicates the on/off state of a channel.  
	If time domain, only 1 channel can be on.
	If stop command issued, this is ignored.
byte 2: unused, can be anything
byte 3: 0x00 - check byte.  If not zero error will be returned

"Return" value - prints a 4 byte value over bluetooth:
	same as was sent if success
	0xFF FF FF FF is not
									
*/
void ProcessUartCommand(unsigned int cmd);

/*
for each channel in use, grabs a copy of time domain data, computes fft, bins it, scales it
and sends over uart.  Also sends the control byte at the end
*/
void ComputeAndSendTransforms(void);

void sendFFTData(unsigned char transformBins[], unsigned char transformScalingValue);

void processDataReadyTimeDomain(void);

void processDataReadyFrequencyDomain(void);

#endif
