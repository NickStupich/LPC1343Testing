#include "fft.h"
#include "uart.h"

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
			//uart_write(((fft[binIndeces[i][j]]/2)>>20) & 0xFF);
			fft[i] += fft[binIndeces[i][j]]/(1<<14);//2;	//avoid int overflow
		}
		//uart_write((fft[i]>>20) & 0xFF);
		//uart_write(CONTROL_BYTE);
		
		if(max < fft[i])
			max = fft[i];
		
	}
	
	/*for(i=0;i<FFT_BIN_COUNT;i++)
	{
		uart_write(fft[i]);
	}*/
	
	//scale back if needed
	max /= MAX_PROTOCOL_VALUE;
	
	//avoid overflowing or underflowing
	if(max > MAX_PROTOCOL_VALUE)
		max = MAX_PROTOCOL_VALUE;
	else if(max == 0)
		max = 1;
	
	for(i=0;i<FFT_BIN_COUNT;i++)
	{
		fft[i] /= max;
		if(fft[i] > MAX_PROTOCOL_VALUE)
			fft[i] = MAX_PROTOCOL_VALUE;
		
		bins[i] = (unsigned char) fft[i];
		/*
		bins[i] = fft[i] / max;
		//uart_write(fft[i]);
		//uart_write(max);
		//uart_write(bins[i]);
		//uart_write(CONTROL_BYTE);
		
		//avoid overflow
		if(bins[i] > MAX_PROTOCOL_VALUE)
			bins[i] = MAX_PROTOCOL_VALUE;*/
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
