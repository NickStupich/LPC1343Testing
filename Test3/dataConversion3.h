
#include "testingData.h"
	
	
//need to bitshift by 8 to get the signed bit proper.
//Divide by 2 to decrease amplitude to avoid overflow while doing an fft

#define SCALE_INTEGER(x)			(x>>1)


#pragma pack(1)

typedef struct{
	unsigned char bytes[4];
} channelBytes;

typedef union{
	int value;
	channelBytes raw;
} channelUnion;

#pragma pack()


channelUnion cu[9];
void testDataConversion()	//182 cycles
{	
	unsigned char* dataPtr;
	channelUnion* cuPtr;
	cuPtr	= &cu[0];
	dataPtr = &_rawData[0];
		
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	cuPtr->raw.bytes[3] = *(dataPtr++);
	cuPtr->raw.bytes[2] = *(dataPtr++);
	(*(cuPtr++)).raw.bytes[1] = *(dataPtr++);
	
	channelData[8][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[7][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[6][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[5][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[4][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[3][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[2][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[1][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	channelData[0][channelIndex] = SCALE_INTEGER((*(--cuPtr)).value);
	
}
