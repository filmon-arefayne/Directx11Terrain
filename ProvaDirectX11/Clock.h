#pragma once

#include <Windows.h>

class Clock
{

private:

	__int64 tempoIniziale, tempoCorrente,frequenza;

	float secondsPerCount;
public:
	Clock();
	~Clock();

	float update();
};

