#include "pll.h"
#include "fft3.h"

#define TMR32B0TC 			(*((volatile unsigned long *) 0x40014008))
#define TMR32B0TCR 			(*((volatile unsigned long *) 0x40014004))
#define SYSAHBCLKCTRL		(*((volatile unsigned long *) 0x40048080))
/*
//sin(2*pi*x/64) * 2.4
float testData[128] = {
0.0,
0.235241136791,
0.468216772839,
0.696683225411,
0.918440237676,
1.13135216838,
1.33336855925,
1.52254388199,
1.69705627485,
1.85522508807,
1.99552706953,
2.11661103444,
2.21731087803,
2.29665680576,
2.35388467297,
2.38844334401,
2.4,
2.38844334401,
2.35388467297,
2.29665680576,
2.21731087803,
2.11661103444,
1.99552706953,
1.85522508807,
1.69705627485,
1.52254388199,
1.33336855925,
1.13135216838,
0.918440237676,
0.696683225411,
0.468216772839,
0.235241136791,
2.93915231795e-16,
-0.235241136791,
-0.468216772839,
-0.696683225411,
-0.918440237676,
-1.13135216838,
-1.33336855925,
-1.52254388199,
-1.69705627485,
-1.85522508807,
-1.99552706953,
-2.11661103444,
-2.21731087803,
-2.29665680576,
-2.35388467297,
-2.38844334401,
-2.4,
-2.38844334401,
-2.35388467297,
-2.29665680576,
-2.21731087803,
-2.11661103444,
-1.99552706953,
-1.85522508807,
-1.69705627485,
-1.52254388199,
-1.33336855925,
-1.13135216838,
-0.918440237676,
-0.696683225411,
-0.468216772839,
-0.235241136791,
-5.87830463591e-16,
0.235241136791,
0.468216772839,
0.696683225411,
0.918440237676,
1.13135216838,
1.33336855925,
1.52254388199,
1.69705627485,
1.85522508807,
1.99552706953,
2.11661103444,
2.21731087803,
2.29665680576,
2.35388467297,
2.38844334401,
2.4,
2.38844334401,
2.35388467297,
2.29665680576,
2.21731087803,
2.11661103444,
1.99552706953,
1.85522508807,
1.69705627485,
1.52254388199,
1.33336855925,
1.13135216838,
0.918440237676,
0.696683225411,
0.468216772839,
0.235241136791,
8.81745695386e-16,
-0.235241136791,
-0.468216772839,
-0.696683225411,
-0.918440237676,
-1.13135216838,
-1.33336855925,
-1.52254388199,
-1.69705627485,
-1.85522508807,
-1.99552706953,
-2.11661103444,
-2.21731087803,
-2.29665680576,
-2.35388467297,
-2.38844334401,
-2.4,
-2.38844334401,
-2.35388467297,
-2.29665680576,
-2.21731087803,
-2.11661103444,
-1.99552706953,
-1.85522508807,
-1.69705627485,
-1.52254388199,
-1.33336855925,
-1.13135216838,
-0.918440237676,
-0.696683225411,
-0.468216772839,
-0.235241136791
};


*/

long testData[128] = {
	0,
12100,
24085,
35837,
47244,
58196,
68588,
78319,
87296,
95432,
102649,
108878,
114058,
118140,
121083,
122861,
123456,
122861,
121083,
118140,
114058,
108878,
102649,
95432,
87296,
78319,
68588,
58196,
47244,
35837,
24085,
12100,
0,
-12100,
-24085,
-35837,
-47244,
-58196,
-68588,
-78319,
-87296,
-95432,
-102649,
-108878,
-114058,
-118140,
-121083,
-122861,
-123456,
-122861,
-121083,
-118140,
-114058,
-108878,
-102649,
-95432,
-87296,
-78319,
-68588,
-58196,
-47244,
-35837,
-24085,
-12100,
0,
12100,
24085,
35837,
47244,
58196,
68588,
78319,
87296,
95432,
102649,
108878,
114058,
118140,
121083,
122861,
123456,
122861,
121083,
118140,
114058,
108878,
102649,
95432,
87296,
78319,
68588,
58196,
47244,
35837,
24085,
12100,
0,
-12100,
-24085,
-35837,
-47244,
-58196,
-68588,
-78319,
-87296,
-95432,
-102649,
-108878,
-114058,
-118140,
-121083,
-122861,
-123456,
-122861,
-121083,
-118140,
-114058,
-108878,
-102649,
-95432,
-87296,
-78319,
-68588,
-58196,
-47244,
-35837,
-24085,
-12100
};
unsigned long start, end, elapsed, loop;

long testOutput[130];

void PerformanceTest()
{	
	//enable timer0
	SYSAHBCLKCTRL |= (1<<9); //power up timer 0
	TMR32B0TCR = 0x1;	//enable for counting
	
	start	= TMR32B0TC;
	//TESTFFT(testData, 128);
	//FFT128Real_32b(testOutput, testData);
	//magnitude32_32bIn(&testOutput[2], 128/2-1);
	//then look at even indeces of testOutput only
	
	realFFT128(testData, testOutput);
	
	end = TMR32B0TC;
	elapsed = end - start;
}

int main()
{
	PerformanceTest();
//	TESTFFT(testData, 128);
	while(1);
}


