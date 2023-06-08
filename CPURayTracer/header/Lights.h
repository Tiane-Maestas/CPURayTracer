#pragma once
#include "RenderingPipelineConfig.h"

class Light 
{
public:
	vec3 color;
	Light(float x, float y, float z, float r, float g, float b) { m_pos = vec4(x, y, z, 1); color = vec3(r, g, b); }
	virtual ~Light() {}
	// Getters
	vec4 GetPosition() const { return m_pos; }
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	// This will transform light position by current 'transf'.
	void Transform();
protected:
	vec4 m_pos;
	mat4 m_transf = mat4(1.0);
};

class Point : public Light 
{
public:
	float intensity = 1.0f;
	vec3 attenuation = vec3(0, 0, 1); // Const, linear, quadratic.
	Point(float x, float y, float z, float r, float g, float b) : Light(x, y, z, r, g, b) {}
	~Point() {}
};

class Directional : public Light 
{
public:
	static vec3 attenuation;
	Directional(float x, float y, float z, float r, float g, float b) : Light(x, y, z, r, g, b) 
	{ 
		m_pos = vec4(x, y, z, 0); // Directional light w = 0.
		m_pos = normalize(m_pos); // Direction should always be normalized.
	}
	~Directional() {}
};