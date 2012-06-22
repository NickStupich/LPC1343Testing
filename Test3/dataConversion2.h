#include "testingData.h"

void testDataConversion() //194 cycles
{
	unsigned char* dataPtr = &_rawData[0];
	unsigned char buf[ADS_DATA_LENGTH];
	unsigned char* pt;
	
	buf[2] = *(dataPtr++);
	buf[1] = *(dataPtr++);
	buf[0] = *(dataPtr++);
	
	buf[5] = *(dataPtr++);
	buf[4] = *(dataPtr++);
	buf[3] = *(dataPtr++);
	
	buf[8] = *(dataPtr++);
	buf[7] = *(dataPtr++);
	buf[6] = *(dataPtr++);
	
	buf[11] = *(dataPtr++);
	buf[10] = *(dataPtr++);
	buf[9] = *(dataPtr++);
	
	buf[14] = *(dataPtr++);
	buf[13] = *(dataPtr++);
	buf[12] = *(dataPtr++);
	
	buf[17] = *(dataPtr++);
	buf[16] = *(dataPtr++);
	buf[15] = *(dataPtr++);
	
	buf[20] = *(dataPtr++);
	buf[19] = *(dataPtr++);
	buf[18] = *(dataPtr++);	
	
	buf[23] = *(dataPtr++);
	buf[22] = *(dataPtr++);
	buf[21] = *(dataPtr++);
	
	buf[26] = *(dataPtr++);
	buf[25] = *(dataPtr++);
	buf[24] = *(dataPtr++);
	
	pt = &buf[3];
	
	channelData[0][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[1][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[2][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[3][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[4][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[5][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[6][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
	channelData[7][channelIndex] = ((*(pt++)<<24) | (*(pt++)<<16) | (*(pt++)<<8))>>1;
}
