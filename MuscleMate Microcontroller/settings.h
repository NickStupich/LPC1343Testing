#ifndef _SETTINGS
#define _SETTINGS

/* General settings on fft related stuff */
#define 					BUFFER_LENGTH								128
#define						SAMPLES_PER_SECOND					768
#define 					NUM_CHANNELS								8
#define 					FFT_FREQUENCY								30
#define 					FFT_BIN_COUNT								10

/* Data protocol stuff.  Should match values on the other end of bluetooth*/
#define 					CONTROL_BYTE								0xFF

/* ADS sampling rate stuff */
#define 					ADS_SPS_FFT									SPS_8k
#define 					ADS_SPS_TIME								SPS_2k

/* ADS physical connection settings */
#define 					DATA_READY_WIRE_PORT				2
#define						DATA_READY_WIRE_PIN					10

/* Interrupt priorities. range 0x0 - 0x1F.  Should be unique */
/*
#define 					INTERRUPT_PRI_GPIO_0				0x1F
#define 					INTERRUPT_PRI_GPIO_1				0x1E
#define 					INTERRUPT_PRI_GPIO_2				0x1D
#define 					INTERRUPT_PRI_GPIO_3				0x1C
*/
#define						INTERRUPT_PRI_DRDY					0x1F
#define 					INTERRUPT_PRI_UART					0x1A
#define 					INTERRUPT_PRI_FFT_TIMER			0x18
#define 					INTERRUPT_PRI_ASYNC_TIMER		0x0F

/* Run modes */
enum RunMode{
	RUN_MODE_TIME_DOMAIN = 1,
	RUN_MODE_FREQ_DOMAIN = 2,
};

#endif
