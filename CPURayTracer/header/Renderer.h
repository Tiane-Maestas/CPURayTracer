#pragma once
#include <FreeImage.h>
#include "RayTracer.h"
#include "Timer.h"

class Renderer 
{
public:
	Renderer(Scene* scene, RenderingOptions options);
	~Renderer();
	// Builds 'imagePixels'
	void RenderScene();
	// Saves 'imagePixels' to a PNG file.
	void SaveImage();
private:
	Scene* m_scene;
	vector<shared_ptr<uint8_t[]>> m_imageBlocks;
	uint8_t* m_imagePixels;
	uint32_t m_totNumBytes;
	RenderingOptions m_options;
	uint32_t m_numThreads = 1;
	uint32_t m_blockSize = 0;
	// Combines all blocks into one array of pixels.
	void ConstructPixelsFromBlocks();
};