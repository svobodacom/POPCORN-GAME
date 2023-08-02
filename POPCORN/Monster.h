#include <chrono>

using namespace std::chrono;

class Stopwatch
{
private:
	using TimePoint = time_point<steady_clock>;

	TimePoint time;
	bool isRunning = false;

public:

	void Start();
	milliseconds Stop();



};