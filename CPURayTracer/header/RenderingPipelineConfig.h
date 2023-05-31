#pragma once
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

const bool UseBVHAcceleration = true;

enum ThreadAmount 
{
	None,
	Single,
	Half,
	Maximum
};

// Default Rendering Options.
struct RenderingOptions
{
	uint32_t ImageWidth = 640;
	uint32_t ImageHeight = 480;
	uint32_t MaxRayDepth = 5;
	uint32_t RaysPerPixel = 5;
	ThreadAmount NumberOfUsedThreads = ThreadAmount::Maximum;
	std::string OutputFileName = "RenderedImage.png";
	vec3 BackgroundColor = vec3(0, 0, 0);
	bool UseSkyBox = true;
};

namespace RenderingStatistics
{
	static bool PrintTimers = true;
	static bool SaveStatistics = false; // Significantly Impacts Performance.
	extern std::atomic<uint64_t> RaysCasted;
	extern std::atomic<uint64_t> NumTriTests;
	extern std::atomic<uint64_t> NumSphTests;
	extern std::atomic<uint64_t> NumElpTests;

	inline void Print()
	{
		if (!SaveStatistics) return;
		std::cout << "----- Statistics -----" << std::endl;
		std::cout << "# Rays Casted: " << RaysCasted << std::endl;
		std::cout << "# Tri Tests: " << NumTriTests << std::endl;
		std::cout << "# Sph Tests: " << NumSphTests << std::endl;
		std::cout << "# Elp Tests: " << NumElpTests << std::endl;
	}
}

namespace ProgressReport
{
	static bool ReportProgress = false; // Significantly Impacts Performance.
	extern std::atomic<uint64_t> TotalBlocks;
	extern std::atomic<uint64_t> RenderedBlocks;

	inline void Print() 
	{
		std::cout << "Image Percent: " << (float)(RenderedBlocks) / (float)TotalBlocks << std::endl;
	}
}