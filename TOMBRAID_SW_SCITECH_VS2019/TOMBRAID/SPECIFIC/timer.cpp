#include "timer.h"

// new for TR1
double LdFreq;

void Init_Timer()
{
	LARGE_INTEGER fq;

	if (!QueryPerformanceFrequency(&fq))
		return;

	//количество тикoв (count) в одну секунду
	LdFreq = (double)fq.LowPart + (double)fq.HighPart * (double)0xffffffff;
	
	//число тиков, приход€щеес€ на 1 / 60 секунды
	LdFreq /= 60.0;
}

int Sync(void)
{
	static double LdSync;
	int nFrames;
	LARGE_INTEGER ct;
	double dCounter;

	QueryPerformanceCounter(&ct);

	//dCounter количество тиков с начала старта системы
	dCounter = (double)ct.LowPart +	(double)ct.HighPart * (double)0x100000000;

	//превращаем dCounter в количество кадров прошедших
	//с начала старта системы
	//LdFreq количество тиков за 1/60 секудны
	dCounter /= LdFreq;

	//сколько кадров (1/60 секунды)
	//прошло с момента предыдущего вызова Sync()
	//long(dCounter) и long(LdSync) Ч берЄм только целые части
	//(отбрасываем дробные, чтобы считать кадры целыми)
	nFrames = long(dCounter) - long(LdSync);

	LdSync = dCounter;

	//при нормальной частоте вызова(60 FPS) nFrames == 1
	//ѕри 60 FPS Ч nFrames == 1
	//ѕри 30 FPS Ч nFrames == 2
	//ѕри 15 FPS Ч nFrames == 4
	return nFrames;
}
