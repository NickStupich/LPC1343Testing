/*

#define TESTFFT(a, b)		realft(a-1, b)
#define SWAP(a,b) 			tempr=(a);(a)=(b);(b)=tempr

const float sinData[9] = {
-1,
1.22464679915e-16,
1.0,
0.707106781187,
0.382683432365,
0.195090322016,
0.0980171403296,
0.0490676743274,
0.0245412285229
};
unsigned char nsin;


void four1(float data[], unsigned long nn)
{
	unsigned long n,mmax,m,j,istep,i;
	float wtemp,wr,wpr,wpi,wi;
	float tempr,tempi;
	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) { //This is the bit-reversal section of the routine.
		if (j > i) { 
			SWAP(data[j],data[i]); //Exchange the two complex numbers.
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}

	//Here begins the Danielson-Lanczos section of the routine.
	mmax=2;
	while (n > mmax) { //Outer loop executed log2 nn times.

		for(nsin=8; (1<<nsin) > mmax;nsin--) {}
		wpi=sinData[nsin++];
		wtemp = sinData[nsin];
		
		istep=mmax << 1;
		wpr = -2.0f*wtemp*wtemp;

			
		wr=1.0f;
		wi=0.0f;
		for (m=1;m<mmax;m+=2) { //Here are the two nested inner loops.
			for (i=m;i<=n;i+=istep) {

				j=i+mmax; //This is the Danielson-Lanczos formula:
				tempr=wr*data[j]-wi*data[j+1]; 
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;

			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr; //Trigonometric recurrence.
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void realft(float data[], unsigned long n)
{
	unsigned long i,i1,i2,i3,i4,np3;
	float c1=0.5f,c2,h1r,h1i,h2r,h2i;
	float wr,wi,wpr,wpi,wtemp;
	float scale = 2.0f / (float)n;
	
	c2 = -0.5;
	four1(data,n>>1); //The forward transform is here.

	for(nsin=8; (1<<nsin) > n;nsin--) {}
	wpi=sinData[nsin++];
	wtemp = sinData[nsin];
	
	//wtemp=usin(n<<1);
	wpr = -2.0f*wtemp*wtemp;
	//wpi=usin(n);
	wr=1.0f+wpr;
	wi=wpi;
	np3=n+3;

	for (i=2;i<=(n>>2);i++) { //Case i=1 done separately below.
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]); //The two separate transforms are sep-
		h1i=c1*(data[i2]-data[i4]); //arated out of data.
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i; //Here they are recombined to form the true transform of the original real data.
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr; //The recurrence.
		wi=wi*wpr+wtemp*wpi+wi;
	}

	data[1] = (h1r=data[1])+data[2]; //Squeeze the First and last data together to get them all within the original array.
	data[2] = h1r-data[2];

	//scale data
	for(i=1;i<=n;i++)
	{
		data[i] *= scale; 
	}
}
*/