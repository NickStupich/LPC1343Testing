#include "settings.h"

const unsigned char binIndeces[FFT_BIN_COUNT][NUM_BINS_COMBINED] = {
	{ 2, 3},	 //12, 18Hz
	{ 4, 5},   //24, 30Hz
	{ 6, 7},   //36, 42Hz
	{ 8, 9}, 	 //48, 54Hz
	{ 11, 12}, //66, 72Hz
	{ 13, 14}, //78, 84Hz
	{ 15, 16}, //90, 96Hz
	{ 17, 18}, //102, 108Hz
	{ 19, 21}, //114, 126Hz
	{ 22, 23}, //132, 138Hz
};
