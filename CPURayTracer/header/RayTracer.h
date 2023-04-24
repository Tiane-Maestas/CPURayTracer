#pragma once
#include "Scene.h"

struct PixelBlock 
{
	uint32_t xMin, yMin, xMax, yMax;
};

class RayTracer 
{
public:
	// The 'RayTracer' will directly write to 'imagePixels'.
	RayTracer(uint8_t* imagePixels, float width, float height);
	~RayTracer() {}
	// Getters
	void join() { m_thread->join(); }
	// Setters
	void SetDefaultColor(vec4 defaultColor) { m_defaultColor = defaultColor; }
	void SetRayDepth(int maxRayDepth) { m_maxRayDepth = maxRayDepth; }
	void SetRaysPerPixel(int raysPerPixel) { m_raysPerPixel = raysPerPixel; }
	// Constructs a byte array in 'm_imagePixels' as the traced image on the main thread.
	void TraceImage(Scene* scene, const PixelBlock& block);
	// Constructs a byte array in 'm_imagePixels' as the traced image on a seperate thread.
	void TraceImage(Scene* scene, const std::vector<PixelBlock>& blocks);
		
private:
	// Threading
	std::shared_ptr<std::thread> m_thread;
	// Image Properties
	uint8_t* m_imagePixels;
	float m_imageWidth = 120;
	float m_imageHeight = 120;
	vec3 m_defaultColor = vec3(0.0);
	// Ray Tracer Properties
	int m_maxRayDepth = 5;
	int m_raysPerPixel = 1;
	// This will construct the 'm_imagePixels' from a seperate thread and a list of blocks.
	void TraceThreadedImage(Scene* scene, const std::vector<PixelBlock>& blocks);
	// Builds a Ray given the scene's camera and the x and y pixel of the image.
	Ray RayThroughPixel(Scene* scene, float x, float y);
	// Returns the Paramater 't' that tells where the given ray hits the scene.
	Intersection FindIntersection(Scene* scene, const Ray& ray);
	// Returns a color given an intersection in the scene.
	vec3 FindColor(Scene* scene, const Intersection& intersection, int recursiveCall);
	// Gets the color at a pos from the reflected light.
	vec3 ColorFromReflections(Scene* scene, const Ray& reflectionRay, int recursiveCall);
	// Transforms a color in a vec4 to uint8_t* so that you can color the pixel.
	void TransformColor(uint8_t* properColor, vec3 color);
	// Colors each color channel in 'm_imagePixels' given the pixel and color.
	void ColorPixel(int pixel, const uint8_t* channels);
};