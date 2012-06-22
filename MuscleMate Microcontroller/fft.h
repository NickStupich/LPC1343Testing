#ifndef _FFT
#define _FFT

#include "settings.h"

#if BUFFER_LENGTH != 128
#error fft stuff here won't work with anything but 128 sample length
#endif

void FFT128Real_32b(int *y, int *x);
void magnitude32_32bIn(int   *x,int M);

void realFFT128(int *in, int* out);

void combineDataToBins(int *fft, unsigned char bins[], unsigned char* scalingValue);

#endif
