#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <deque>
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

enum Amount {
	None,
	Single,
	Half,
	Maximum
};

struct RenderingOptions
{
	uint32_t ImageWidth = 640;
	uint32_t ImageHeight = 480;
	uint32_t MaxRayDepth = 5;
	uint32_t RaysPerPixel = 5;
	Amount NumberOfUsedThreads = Amount::Maximum;
	std::string OutputFileName = "RenderedImage.png";
	vec3 BackgroundColor = vec3(0, 0, 0);
};

namespace RenderingStatistics
{
	static bool PrintTimers = true;
	static std::atomic<int> numTriTests = 0;
	static std::atomic<int> raysCasted = 0;

	static void Print()
	{
		std::cout << "TODO - Statistics" << std::endl;
	}
}