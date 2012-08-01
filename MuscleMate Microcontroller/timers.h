#ifndef _TIMERS
#define _TIMERS

/* Timer to get the system to run FFTs at a predetermined rate */
void StartFFTTimer(void);
void StopFFTTimer(void);
void FFTTimerInit(void);

/* Timer used to be able to call delay(x)  to wait x microseconds */
void DelayTimerInit(void);
void delay(unsigned short us);

/* Timer used to set up a function to run at a later time.  Only deals with 1 async function at a time (for now)*/
void AsyncTimerFunctionCall(unsigned int delay, void (*callbackFunc)());

void pwdnTimerInit(void);
void startPwdnTimer(void);
void stopPwdnTimer(void);

#endif
