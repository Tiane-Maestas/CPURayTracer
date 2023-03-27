#pragma once
#include <FreeImage.h>
#include "Scene.h"

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
	Scene* scene;
	BYTE* imagePixels;
	RenderingOptions options;
};