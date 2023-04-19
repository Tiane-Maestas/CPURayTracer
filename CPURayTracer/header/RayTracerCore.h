#pragma once
#include "RenderingPipelineConfig.h"
#include "Materials.h"

class Ray
{
public:
	// I want to make sure I force homogeneous coordinate to 0 for 'dir'.
	Ray(vec4 pos, vec3 dir) { m_pos = pos; m_dir = vec4(dir, 0); }
	~Ray() {}
	vec4 getPosition() { return m_pos; }
	vec4 getDirection() { return m_dir; }
	vec4 At(float t);
	void Transform(const mat4& transf);
private:
	vec4 m_pos;
	vec4 m_dir;
};

// Any object that can be rendered to the scene must implement this interface.
class Traceable
{
	// Given a ray, return the world space position it was hit at.
	// If not hit return vec4(FLT_MAX);
	virtual vec4 Intersect(Ray& ray) = 0;
	// Get normal at a position on object.
	virtual vec3 getNormal(vec4 pos) = 0;
	// Specifies how to color object with no texture.
	Material material;
};