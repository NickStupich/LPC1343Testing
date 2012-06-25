
enum AdsSampleRates{
	SPS_32k = 0x0,
	SPS_16k = 0x1,
	SPS_8k = 0x2,
	SPS_4k = 0x3,
	SPS_2k = 0x4,
	SPS_1k = 0x5,
	SPS_500 = 0x6
};

/* TODO: should start() be removed and just have init() which starts it?*/

/* 
sets up and starts communication with the ads chip over spi
sample frequency on ads is set using <sampleRate> - defined above
all channels run by default, maybe turn one off later?
*/
void initSpiWithAds(enum AdsSampleRates sampleRate);

/*
Starts the ads sending stuff
*/
void startSpiWithAds(void);

/* 
stops the ads sending stuff
*/
void stopSpiWithAds(void);
