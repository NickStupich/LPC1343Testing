#ifndef _CORE_FUNCTIONS
#define _CORE_FUNCTIONS


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


void ComputeAndSendTransforms(void);

void sendFFTData(unsigned char transformBins[], unsigned char transformScalingValue);
#endif
