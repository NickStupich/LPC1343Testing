#include "settings.h"

enum AdsSampleRates{
	SPS_32k = 0x0,
	SPS_16k = 0x1,
	SPS_8k = 0x2,
	SPS_4k = 0x3,
	SPS_2k = 0x4,
	SPS_1k = 0x5,
	SPS_500 = 0x6
};

/* 
sets up and starts communication with the ads chip over spi
sample frequency on ads is set using <sampleRate> - defined above
all channels run by default, maybe turn one off later?
*/
void initSpiWithAds(enum RunMode runMode);

/* stops the ads sending stuff */
void stopSpiWithAds(void);

/* sets up the external interrupt pin */
void initDRDYInterrupt(void);

/* read a byte over spi.  Also writes 0x00*/
unsigned char SPI0_Read(void);

/*write a byte over spi. length is the number of bytes you would like to continiously send */
void SPI0_Write(unsigned char Data);

/* generally start up SPI*/
void InitSPI(void);
