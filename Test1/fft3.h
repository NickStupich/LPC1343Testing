
void FFT128Real_32b(int *y, int *x);
void magnitude32_32bIn(int   *x,int M);

void realFFT128(int *in, int*out)
{
	int i;
	FFT128Real_32b(out, in);
	magnitude32_32bIn(&out[2], 128/2-1);
	for(i=0;i<128/2;i++)
		out[i] = out[i<<1];	
}
