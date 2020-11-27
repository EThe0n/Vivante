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
		double fps = 1.0 / newTime_ms;
		++frameCounter;
		currentFPS = (unsigned int)fps;
		maxFPS = std::max(currentFPS, maxFPS);
		minFPS = std::min(currentFPS, minFPS);
		sumFPS += currentFPS;
	}

	unsigned int getAvgFPS()
	{
		return sumFPS / frameCounter;
	}

	unsigned int currentFPS;
	unsigned int maxFPS;
	unsigned int minFPS;
	unsigned int sumFPS;
	int frameCounter;
};