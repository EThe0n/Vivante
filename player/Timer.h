#pragma once

#include <climits>
#include <algorithm>

struct Timer
{
	Timer()
	{
		reset();
	}

	void reset()
	{
		currentFPS   = 0;
		maxFPS 		 = 0;
		minFPS 		 = (double)UINT_MAX;
		sumFPS 		 = 0;
		frameCounter = 0;
	}

	void update(double newTime_ms)
	{
		++frameCounter;
		currentFPS = 1000.0 / newTime_ms;
		maxFPS = std::max(currentFPS, maxFPS);
		minFPS = std::min(currentFPS, minFPS);
		sumFPS += currentFPS;
	}

	double getAvgFPS()
	{
		return sumFPS / frameCounter;
	}

	double currentFPS;
	double maxFPS;
	double minFPS;
	double sumFPS;
	int frameCounter;
};