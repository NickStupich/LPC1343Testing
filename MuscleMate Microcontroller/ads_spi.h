#include "settings.h"

/* 
sets up and starts communication with the ads chip over spi
sample frequency on ads is set using <sampleRate> - defined above
all channels run by default, maybe turn one off later?
*/
void initSpiWithAds(enum RunMode runMode);

/* stops the ads sending stuff */
void stopAdsConversions(void);

/* sets up the external interrupt pin */
void initDRDYInterrupt(void);

/* read a byte over spi.  Also writes 0x00*/
unsigned char SPI0_Read(void);

/*write a byte over spi. length is the number of bytes you would like to continiously send */
void SPI0_Write(unsigned char Data);

/* generally start up SPI*/
void InitSPI(void);

/* set up pins, right at startup.  This allows us to do stuff like powering down the ads right away */
void InitPinsForAdsCommunication(void);
