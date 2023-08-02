#include "Monster.h"

void Stopwatch::Start()
{
	time = high_resolution_clock().now();
	isRunning = true;
}

milliseconds Stopwatch::Stop()
{
	if (isRunning)
	{
		isRunning = false;
		return duration_cast<milliseconds>(high_resolution_clock().now() - time);
	}
	else
	{
		return duration_cast<milliseconds>(time.time_since_epoch());
	}
}
