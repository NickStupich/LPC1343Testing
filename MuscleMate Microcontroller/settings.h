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
#define						UART_CMD_LENGTH							0x4

#define 					UART_CMD_CMD_MASK						0xFF000000
#define 					UART_CMD_CHANNELS_MASK			0xFF0000
#define 					UART_CMD_CHECK_MASK					0xFF

#define 					UART_CMD_STOP								0x1
#define 					UART_CMD_START_FFT					0x2
#define 					UART_CMD_START_TIME					0x4
#define						UART_CMD_ENTER_ISP					0x8

#define 					UART_GET_CMD(x)							((x & UART_CMD_CMD_MASK)>>24)
#define						UART_GET_CHANNELS(x)				((x & UART_CMD_CHANNELS_MASK) >> 16)
#define						UART_GET_CHECK(x)						((x & UART_CMD_CHECK_MASK))

/* ADS sampling rate stuff */

enum AdsSampleRates{
	SPS_32k = 0x0,
	SPS_16k = 0x1,
	SPS_8k = 0x2,
	SPS_4k = 0x3,
	SPS_2k = 0x4,
	SPS_1k = 0x5,
	SPS_500 = 0x6
};

#define 					ADS_SPS_FFT									SPS_8k
#define 					ADS_SPS_TIME								SPS_500

#if ADS_SPS_FFT == SPS_32k
#define ADS_SPS_NUMERICAL			32000
#elif ADS_SPS_FFT == SPS_16k
#define ADS_SPS_NUMERICAL			16000
#elif ADS_SPS_FFT == SPS_8k
#define ADS_SPS_NUMERICAL			8000
#elif ADS_SPS_FFT == SPS_4k
#define ADS_SPS_NUMERICAL			4000
#elif ADS_SPS_FFT == SPS_2k
#define ADS_SPS_NUMERICAL			2000
#elif ADS_SPS_FFT == SPS_1k
#define ADS_SPS_NUMERICAL			1000
#elif ADS_SPS_FFT == SPS_500
#define ADS_SPS_NUMERICAL			500
#endif

/* ADS physical connection settings */
#define 					DATA_READY_WIRE_PORT				0
#define						DATA_READY_WIRE_PIN					5

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

/* Event processing stuff.  Event processing loop is in main */
#define						EVENT_PROCESS_UART_CMD			0x0
#define						EVENT_PROCESS_UART_MASK			EVENT_CMD_TO_MASK(EVENT_PROCESS_UART_CMD)

#define						EVENT_CMD_TO_MASK(x)				(1<<x)
#define						EVENT_PROCESS_NUM_CMDS			0x1

#endif
