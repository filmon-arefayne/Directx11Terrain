#include "Clock.h"



Clock::Clock()
{
	frequenza = tempoIniziale = tempoCorrente = 0;
	secondsPerCount = 0.0f;


	QueryPerformanceFrequency((LARGE_INTEGER*)&frequenza);
	secondsPerCount = 1.0 / (double)frequenza;
	
	QueryPerformanceCounter((LARGE_INTEGER*)&tempoIniziale);
}


Clock::~Clock()
{
}



float Clock::update()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	tempoCorrente = currTime;


	float deltaTime = (tempoCorrente - tempoIniziale)*secondsPerCount;

	tempoIniziale = tempoCorrente;


	if (deltaTime < 0.0)
	{
		deltaTime = 0.0;
	}
	return deltaTime;
	
}