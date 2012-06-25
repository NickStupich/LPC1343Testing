#ifndef _TIMERS
#define _TIMERS

void StartFFTTimer(void);
void StopFFTTimer(void);
void FFTTimerInit(void);
void DelayTimerInit(void);
void delay(unsigned short us);
void AsyncTimerFunctionCall(unsigned int delay, void (*callbackFunc)());

#endif
