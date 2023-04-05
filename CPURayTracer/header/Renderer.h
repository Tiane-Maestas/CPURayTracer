#pragma once
#include <FreeImage.h>
#include "RayTracer.h"
#include "Timer.h"

class Renderer 
{
public:
	Renderer(std::shared_ptr<Scene> scene, RenderingOptions options);
	~Renderer();
	// Builds 'm_imagePixels'
	void RenderScene();
	// Saves 'm_imagePixels' to a PNG file.
	void SaveImage();
private:
	std::shared_ptr<Scene> m_scene;
	RenderingOptions m_options;
	uint8_t* m_imagePixels;
	uint32_t m_totNumBytes;
	// Threading Properties
	uint32_t m_numThreads = 1;
	uint32_t m_blockSize = 0;
};