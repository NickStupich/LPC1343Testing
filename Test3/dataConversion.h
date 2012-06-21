
#include "testingData.h"
	
	
//need to bitshift by 8 to get the signed bit proper.
//Divide by 2 to decrease amplitude to avoid overflow while doing an fft

//#define _24TO32BITINTEGER(x)		((x<<8)/2)					//- 452 cycles


#define _24TO32BITINTEGER(x)		(((signed int)(x<<8)) >> 1)		//442 cycles

#pragma pack(1)
typedef struct{
	unsigned int status:24;
	int d0:24;
	int d1:24;
	int d2:24;
	int d3:24;
	int d4:24;
	int d5:24;
	int d6:24;
	int d7:24;
} usefulData;

typedef union{
	usefulData useful;
	unsigned char raw[ADS_DATA_LENGTH];
} adsUnion;

#pragma pack()

void testDataConversion()
{
	int x1, x2;
	adsUnion au;
	usefulData useful;
	int i;
	for(i=0;i<ADS_DATA_LENGTH;i++)
	{
		au.raw[i] = _rawData[i];
	}
	
	x1 = sizeof(au);
	x2 = sizeof(useful);
	
	channelData[0][channelIndex] = _24TO32BITINTEGER(au.useful.d0);
	channelData[1][channelIndex] = _24TO32BITINTEGER(au.useful.d1);
	channelData[2][channelIndex] = _24TO32BITINTEGER(au.useful.d2);
	channelData[3][channelIndex] = _24TO32BITINTEGER(au.useful.d3);
	channelData[4][channelIndex] = _24TO32BITINTEGER(au.useful.d4);
	channelData[5][channelIndex] = _24TO32BITINTEGER(au.useful.d5);
	channelData[6][channelIndex] = _24TO32BITINTEGER(au.useful.d6);
	channelData[7][channelIndex] = _24TO32BITINTEGER(au.useful.d7);
	
}
