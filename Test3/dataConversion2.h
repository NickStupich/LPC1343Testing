#include "testingData.h"

unsigned char dataBuf[ADS_DATA_LENGTH];

void testDataConversion()
{
	int i;
	
	for(i=0;i<ADS_DATA_LENGTH;i++)
	{
		dataBuf[i] = _rawData[i];
	}
	/*
	unsigned char* db = &dataBuf[0];
	for(i=0;i<ADS_DATA_LENGTH;i++)
		*(db++) = _rawData[i];
	*/
}
