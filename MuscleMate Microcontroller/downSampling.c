#include "downSampling.h"
#include "string.h"
#include "lpc13xx.h"

float interpolationCounter = 0.0f;
float downSampleRate = ((float)ADS_SPS_NUMERICAL) / ((float)SAMPLES_PER_SECOND);
int lastSamples[NUM_CHANNELS];

int performDownSampling(int input[NUM_CHANNELS], int output[NUM_CHANNELS])
{
	int i;
	float otherWeight;
	interpolationCounter+= 1.0;
	if(interpolationCounter > downSampleRate)
	{
		//it's time to take a sample.  Do this by interpolating between the current and previous sample values
		
		//next line assumes that downSampleRate > 1, which should always be the case(compiler will error otherwise)
		//also assumes numbers are reasonably small and floats are perfect
		interpolationCounter -= downSampleRate;
		
		//linear weighting - interpolationCounter becomes the weight of the previous sample, 
		//and (1.0 - interpolationCounter) is the weight of the current sample
		otherWeight = 1.0 - interpolationCounter;
		
		for(i=0;i<NUM_CHANNELS;i++)
		{
			output[i] = interpolationCounter * lastSamples[i] + otherWeight * input[i];
		}
		
		return 1;
	}
	
	//if we need to interpolate next time around
	else if(interpolationCounter - downSampleRate < 1)	
	{
		//memcpy(&lastSamples[0], &input[0], NUM_CHANNELS * sizeof(int));
		for(i=0;i<NUM_CHANNELS;i++)
			lastSamples[i] = input[i];
	}
	
	return 0;
}

