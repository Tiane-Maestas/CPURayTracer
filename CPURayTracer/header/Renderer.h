#pragma once
#include "RayTracer.h"
#include "Timer.h"
#include "Image.h"

class Renderer 
{
public:
	Renderer(std::shared_ptr<Scene> scene, RenderingOptions options);
	~Renderer() {}
	// Builds 'm_imagePixels'
	void RenderScene();
	// Saves 'm_imagePixels' to a PNG file.
	void SaveImage();
private:
	std::shared_ptr<Scene> m_scene;
	RenderingOptions m_options;
	// Pixel Buffer
	Image m_image = Image(0, 0);
	// Threading Properties
	uint32_t m_numThreads = 1;
	uint32_t m_blockSize = 0;
};