#pragma once
#include <chrono>

class Stopwatch
{
	using TimePoint = std::chrono::steady_clock::time_point;
	using DurationType = std::chrono::milliseconds;
private:
	bool	  _do_start;
	TimePoint _start;
	DurationType _elapsed;

private:
	Stopwatch(TimePoint&& start)
		:
		_do_start(true),
		_start(std::move(start)),
		_elapsed{}
	{
	}
public:
	Stopwatch()
		:
		_do_start(false),
		_start{},
		_elapsed{}
	{
	}

	static Stopwatch StartNow()
	{
		return Stopwatch(std::chrono::steady_clock::now());
	}

	void Start()
	{
		_start = std::chrono::steady_clock::now();
		_do_start = true;
	}

	void Restart()
	{
		_start = std::chrono::steady_clock::now();
		_elapsed = {};
		_do_start = true;
	}

	void Stop()
	{
		auto _end = std::chrono::steady_clock::now();
		_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
		_do_start = false;
	}

	long long ElapsedMilliSec() noexcept
	{
		if (_do_start == false)
		{
			return _elapsed.count();
		}

		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
	}
};

