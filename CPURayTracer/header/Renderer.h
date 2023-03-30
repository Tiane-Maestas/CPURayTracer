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
	BYTE* m_imagePixels;
	uint32_t m_numBytes;
	RenderingOptions m_options;
};