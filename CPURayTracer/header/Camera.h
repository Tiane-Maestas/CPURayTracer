#pragma once
#include "RenderingPipelineConfig.h"

class Camera 
{
public:
	Camera(uint64_t eyex, uint64_t eyey, uint64_t eyez, 
		   uint64_t centerx, uint64_t centery, uint64_t centerz, 
		   uint64_t upx, uint64_t upy, uint64_t upz, 
		   uint64_t fovy);
	Camera() {}
	~Camera() {}
	// Getters
	vec4 Position() { return pos; }
	vec4 LookingAt() { return center; }
	vec2 FOV() { return fov; }
	// Setters
	void UpdateTransform(mat4 transf) { this->transf = transf; }
	void SetFOVx(uint64_t imageWidth, uint64_t imageHeight); // This is so that the 'World Aspect Ratio' is always 1.
	// This will transform camera position and up vector by its current transform.
	void Transform();
	// Returns the transform of world space into camera 'eye' space.
	mat4 LookAt();
private:
	vec4 pos = vec4(0.0);
	mat4 transf = mat4(1.0);
	vec4 center = vec4(0.0);
	vec4 up = vec4(0.0);
	vec2 fov = vec2(0.0);
};