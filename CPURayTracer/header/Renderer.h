#pragma once
#include <FreeImage.h>
#include "RayTracer.h"

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
	RenderingOptions m_options;
};