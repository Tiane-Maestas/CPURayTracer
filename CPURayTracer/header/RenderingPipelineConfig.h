#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <deque>

using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

struct RenderingOptions
{
	uint64_t ImageWidth = 640;
	uint64_t ImageHeight = 480;
	uint32_t MaxRayDepth = 5;
	uint32_t RaysPerPixel = 5;
	string OutputFileName = "RenderedImage.png";
	vec4 BackgroundColor = vec4(0, 0, 0, 0);
};