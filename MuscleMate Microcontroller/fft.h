#ifndef _FFT
#define _FFT

#include "settings.h"

#if BUFFER_LENGTH != 128
#error fft stuff here won't work with anything but 128 sample length
#endif

/* declarations for functions written in assembly*/
void FFT128Real_32b(int *y, int *x);
void magnitude32_32bIn(int   *x,int M);

/* calculates the FFT, combines real and imaginary parts to get magnitude, and condenses the magnitudes into the first half +1 of the data buffer*/
void realFFT128(int *in, int* out);

/* combines fft bins into those that will be sent over bluetooth */
void combineDataToBins(int *fft, unsigned char bins[], unsigned char* scalingValue);

#endif
