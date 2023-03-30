#pragma once
#include <chrono>

#include "RenderingPipelineConfig.h"

class Timer
{
public:
	Timer(string name)
	{
		m_name = name;
		m_startTime = chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		stop();
	}

	void stop()
	{
		chrono::high_resolution_clock::time_point stopTime = chrono::high_resolution_clock::now();
		long long duration = chrono::duration_cast<chrono::milliseconds>(stopTime - m_startTime).count();
		if (RenderingStatistics::PrintTimers) 
		{
			cout << m_name + ": " + to_string(duration) + " [ms]\n";
		}
	}

private:
	string m_name;
	chrono::high_resolution_clock::time_point m_startTime;
};