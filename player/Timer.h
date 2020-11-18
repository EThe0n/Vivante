#pragma once

#include <climits>
#include <algorithm>

struct Timer
{
	Timer():
		currentTime_ms(0.0),
		maxTime_ms(0.0),
		minTime_ms((double)UINT_MAX)
	{}

	void reset()
	{
		currentTime_ms = 0.0;
		maxTime_ms = 0.0;
		minTime_ms = (double)UINT_MAX;
	}

	void update(double newTime_ms)
	{
		currentTime_ms = newTime_ms;
		maxTime_ms = std::max(newTime_ms, maxTime_ms);
		minTime_ms = std::min(newTime_ms, minTime_ms);
	}

	double currentTime_ms;
	double maxTime_ms;
	double minTime_ms;
};