#pragma once
#include "Scene.h"

class Ray {
public:
	Ray(vec4 pos, vec3 dir) { m_pos = pos; m_dir = vec4(dir, 0); } // I want to make sure I force homogeneous coordinate to 0 for 'dir'.
	~Ray() {}
	vec4 At(float t);
	void transform(mat4 transf);
private:
	vec4 m_pos;
	vec4 m_dir;
};

struct Intersection
{
	Ray ray;
	vec4 hitPos;
	const shared_ptr<Mesh> hitMesh = nullptr;
};

class RayTracer {
public:
	RayTracer(float width, float height);
	~RayTracer();
	// Setters
	void setDefaultColor(vec4 defaultColor) { m_defaultColor = defaultColor; }
	void setRayDepth(int maxRayDepth) { m_maxRayDepth = maxRayDepth; }
	void setRaysPerPixel(int raysPerPixel) { m_raysPerPixel = raysPerPixel; }
	// Constructs a byte array in imagePixels as the traced image.
	void traceImage(Scene* scene);
	// Builds a Ray given the scene's camera and the x and y pixel of the image.
	Ray rayThroughPixel(Scene* scene, float x, float y);
	// Returns the Paramater 't' that tells where the given ray hits the scene.
	Intersection findIntersection(Scene* scene, Ray ray);
	// Returns a color given an intersection in the scene.
	vec3 findColor(Scene* scene, Intersection intersection, int recursiveCall);
	// Transforms a color in a vec3 to uint8_t* so that you can color the pixel.
	void transformColor(uint8_t* properColor, vec3 color);
	// Gets the color at a pos from the reflected light.
	vec3 colorFromReflections(Scene* scene, Ray reflectionRay, int recursiveCall);
	// Colors each color channel in imagePixels given the pixel and color.
	void ColorPixel(int pixel, const uint8_t* channels);
		
private:
	uint8_t* m_imagePixels;
	float m_imageWidth = 120;
	float m_imageHeight = 120;
	vec4 m_defaultColor = vec4(0.0);
	int m_maxRayDepth = 5;
	int m_raysPerPixel = 1;
};