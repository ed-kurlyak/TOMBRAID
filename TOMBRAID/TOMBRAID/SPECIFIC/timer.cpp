#include "timer.h"

//new for TR1
double LdFreq;

void Init_Timer()
{
LARGE_INTEGER fq;
	
	if (!QueryPerformanceFrequency(&fq))
		return;

	LdFreq=(double)fq.LowPart+(double)fq.HighPart*(double)0xffffffff;
	LdFreq/=60.0;
}


int Sync(void)
{
	static double LdSync;
	int nFrames;
	LARGE_INTEGER ct;
	double dCounter;
	
	QueryPerformanceCounter(&ct);

	dCounter=(double)ct.LowPart+(double)ct.HighPart*(double)0x100000000;	// WARNING: inaccuracy as HighPart increases and double loses low bit precision?
	dCounter/=LdFreq;

	nFrames=long(dCounter)-long(LdSync);	// Use the non-fractional bits to get the frame count

	LdSync=dCounter;
	return nFrames;
}
