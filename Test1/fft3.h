
void FFT128Real_32b(long *y, long *x);
void magnitude32_32bIn(long   *x,int M);

void realFFT128(long *in, long*out)
{
	int i;
	FFT128Real_32b(out, in);
	magnitude32_32bIn(&out[2], 128/2-1);
	for(i=0;i<128/2;i++)
		out[i] = out[i<<1];	
}
