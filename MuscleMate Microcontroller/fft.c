#include "fft.h"

void combineDataToBins(int *fft, unsigned char bins[], unsigned char* scalingValue)
{
	int i, j;
	int max = 0;
	
	//combine bins, find the max bin value
	for(i=0;i<FFT_BIN_COUNT;i++)
	{
		fft[i] = 0;
		for(j=0;j<NUM_BINS_COMBINED;j++)
		{
			fft[i] += fft[binIndeces[i][j]];
		}
		if(max < fft[i])
			max = fft[i];
	}
	
	//scale back
	max /= MAX_PROTOCOL_VALUE;
	
	//avoid overflowing
	if(max > MAX_PROTOCOL_VALUE)
		max = MAX_PROTOCOL_VALUE;
	
	for(i=0;i<FFT_BIN_COUNT;i++)
	{
		bins[i] = fft[i] / max;
		
		//avoid overflow
		if(bins[i] > MAX_PROTOCOL_VALUE)
			bins[i] = MAX_PROTOCOL_VALUE;
	}
	
	//'return' the scaling value
	*scalingValue = (unsigned char) max;
}

void realFFT128(int *in, int* out)
{
	int i;
	FFT128Real_32b(out, in);
	magnitude32_32bIn(&out[2], 128/2-1);
	for(i=0;i<128/2;i++)
		out[i] = out[i<<1];	
}
