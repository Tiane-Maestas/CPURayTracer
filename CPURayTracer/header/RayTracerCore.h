#pragma once
#include "RenderingPipelineConfig.h"
#include "Materials.h"

class Ray
{
public:
	// I want to make sure I force homogeneous coordinate to 0 for 'dir'.
	Ray(vec4 pos, vec3 dir) { m_pos = pos; m_dir = vec4(dir, 0); }
	~Ray() {}
	vec4 GetPosition() const { return m_pos; }
	vec4 GetDirection() const { return m_dir; }
	vec4 At(float t) const;
	void Transform(const mat4& transf);
private:
	vec4 m_pos;
	vec4 m_dir;
};

struct Intersection
{
	Ray ray;
	vec4 hitPos;
	vec3 normal;
	vec2 uv;
	Material material;
};

// Any object that can be rendered to the scene must implement this interface.
class Traceable
{
public:
	// Given a ray, return the intersection it was closest hit at.
	// If not hit hitPos = vec4(FLT_MAX);
	virtual Intersection Intersect(const Ray& ray) = 0;
	// Must update the object by its current m_transf.
	virtual void Transform() = 0;
	// Getters
	std::string GetName() { return m_name; }
	vec4 GetPosition() { return m_pos; }
	mat4 GetTransform() { return m_transf; }
	// Setters
	void UpdateMaterial(Material material) { m_material = material; }
	void UpdateTransform(mat4 transf) { m_transf = transf; }
protected:
	std::string m_name;
	vec4 m_pos = vec4(0.0);
	mat4 m_transf = mat4(1.0);
	// Specifies how to color object with no texture.
	Material m_material;
};