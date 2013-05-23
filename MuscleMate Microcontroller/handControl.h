#ifndef _HAND_CONTROL
#define _HAND_CONTROL

#include "settings.h"
#include "lpc13xx.h"
#include "events.h"
#include "lpc1343Constants.h"


#define OUTPUT_0_PORT	2
#define OUTPUT_0_PIN 	0
#define OUTPUT_1_PORT 2
#define OUTPUT_1_PIN	1

#define ADJUST_LED_PORT	2
#define ADJUST_LED_PIN	2

#define ADJUST_TOGGLE_PORT	1
#define ADJUST_TOGGLE_PIN		4

#define ADJUST_ENABLE_PORT	1
#define ADJUST_ENABLE_PIN		5

#define OUTPUT_0_CHANNEL	6
#define	OUTPUT_1_CHANNEL	5

#define OUTPUT_0_ADC_SCALE	10
#define	OUTPUT_1_ADC_SCALE	10

extern int output0Threshold;
extern int output1Threshold;

void handControlInit(void);

void outputAdjustState(void);

void SetupADC(void);

#endif
