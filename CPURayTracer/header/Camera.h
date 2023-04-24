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
	vec4 GetPosition() { return m_pos; }
	vec4 GetLookingAt() { return m_center; }
	vec3 GetUp() { return m_up; }
	vec2 GetFOV() { return m_fov; }
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	void ChangeCenter(mat4 transf) { m_center = transf * m_center; }
	void SetFOV(float imageWidth, float imageHeight); // This is so that the 'World Aspect Ratio' is always 1.
	// This will transform camera position and up vector by its current transform.
	// Note: The up vector transform hasn't been tested.
	void Transform();
	// Returns the transform of world space into camera 'eye' space.
	mat4 LookAt();
private:
	vec4 m_pos = vec4(0.0);
	mat4 m_transf = mat4(1.0);
	vec4 m_center = vec4(0.0);
	vec3 m_up = vec4(0.0);
	vec2 m_fov = vec2(0.0);
};