#pragma once
#include <chrono>

#include "RenderingPipelineConfig.h"

class Timer
{
public:
	Timer(std::string name)
	{
		m_name = name;
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		stop();
	}

	void stop()
	{
		std::chrono::high_resolution_clock::time_point stopTime = std::chrono::high_resolution_clock::now();
		long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - m_startTime).count();
		if (RenderingStatistics::PrintTimers) 
		{
			std::cout << m_name + ": " + std::to_string(duration) + " [ms]\n";
		}
	}

private:
	std::string m_name;
	std::chrono::high_resolution_clock::time_point m_startTime;
};