#pragma once
#include "RenderingPipelineConfig.h"

class Light 
{
public:
	Light(uint64_t x, uint64_t y, uint64_t z, uint64_t r, uint64_t g, uint64_t b, uint64_t a) { pos = vec4(x, y, z, 1); color = vec4(r, g, b, a); }
	virtual ~Light() {}
	// Getters
	vec4 getPosition() { return pos; }
	vec4 getColor() { return color; }
	// Setters
	void UpdateTransform(mat4 transf) { this->transf = transf; }
	// This will transform light position by current 'transf'.
	void Transform();
private:
	vec4 pos = vec4(0.0);
	mat4 transf = mat4(1.0);
	vec4 color = vec4(0.0);
};

class Point : public Light 
{
public:
	uint64_t intensity = 1.0f;
	vec3 attenuation = vec3(0, 0, 1); // Const, linear, quadratic.
	Point(uint64_t x, uint64_t y, uint64_t z, uint64_t r, uint64_t g, uint64_t b, uint64_t a) : Light(x, y, z, r, g, b, a) {}
	~Point() {}
};

class Directional : public Light 
{
public:
	static vec3 attenuation;
	Directional(uint64_t x, uint64_t y, uint64_t z, uint64_t r, uint64_t g, uint64_t b, uint64_t a) : Light(x, y, z, r, g, b, a) {}
	~Directional() {}
};