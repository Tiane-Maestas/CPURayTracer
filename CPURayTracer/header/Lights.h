#pragma once
#include "RenderingPipelineConfig.h"

class Light 
{
public:
	Light(float x, float y, float z, float r, float g, float b) { m_pos = vec4(x, y, z, 1); m_color = vec3(r, g, b); }
	virtual ~Light() {}
	// Getters
	vec4 getPosition() { return m_pos; }
	vec3 getColor() { return m_color; }
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	// This will transform light position by current 'transf'.
	void Transform();
protected:
	vec4 m_pos;
	vec3 m_color;
	mat4 m_transf = mat4(1.0);
};

class Point : public Light 
{
public:
	Point(float x, float y, float z, float r, float g, float b) : Light(x, y, z, r, g, b) {}
	~Point() {}
	// Getters
	float getIntensity() { return m_intensity; }
	vec3 getAttenuation() { return m_attenuation; }
	// Setters
	void setIntensity(float intensity) { m_intensity = intensity; }
	void setAttenuation(vec3 attenuation) { m_attenuation = attenuation; }
private:
	float m_intensity = 1.0f;
	vec3 m_attenuation = vec3(0, 0, 1); // Const, linear, quadratic.
};

class Directional : public Light 
{
public:
	static vec3 attenuation;
	Directional(float x, float y, float z, float r, float g, float b) : Light(x, y, z, r, g, b) { m_pos = vec4(x, y, z, 0); } // Directional light w = 0.
	~Directional() {}
};