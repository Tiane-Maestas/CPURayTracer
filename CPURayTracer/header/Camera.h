#pragma once
#include "RenderingPipelineConfig.h"

class Camera 
{
public:
	Camera(float eyex, float eyey, float eyez, 
		   float centerx, float centery, float centerz, 
		   float upx, float upy, float upz, 
		   uint32_t fovy);
	Camera() {}
	~Camera() {}
	// Getters
	vec4 getPosition() { return m_pos; }
	vec4 getLookingAt() { return m_center; }
	vec4 getUp() { return m_up; }
	vec2 getFOV() { return m_fov; }
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	void setFOVx(float imageWidth, float imageHeight); // This is so that the 'World Aspect Ratio' is always 1.
	// This will transform camera position and up vector by its current transform.
	void Transform();
	// Returns the transform of world space into camera 'eye' space.
	mat4 LookAt();
private:
	vec4 m_pos = vec4(0.0);
	mat4 m_transf = mat4(1.0);
	vec4 m_center = vec4(0.0);
	vec4 m_up = vec4(0.0);
	vec2 m_fov = vec2(0.0);
};