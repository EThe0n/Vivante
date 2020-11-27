#pragma once

#include <climits>
#include <algorithm>

struct Timer
{
	Timer():
		currentTime_ms	(0.0),
		totalTime_ms	(0.0),
		maxTime_ms		(0.0),
		minTime_ms		((double)UINT_MAX),
		counter			(0)
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
		totalTime_ms += newTime_ms;
		++counter;
	}

	double getAverageTime()
	{
		return totalTime_ms / (double)counter;
	}

	double getMaxFPS()
	{
		return (double)counter / minTime_ms * 1000.0;
	} 

	double getMinFPS()
	{
		return (double)counter / maxTime_ms * 1000.0;
	}

	double getAvgFPS()
	{
		return (double)counter / getAverageTime() * 1000.0;
	}

	double getCurrentFPS()
	{
		return (double)counter / currentTime_ms * 1000.0;
	}

	double currentTime_ms;
	double maxTime_ms;
	double minTime_ms;
	double totalTime_ms;
	int counter;
};