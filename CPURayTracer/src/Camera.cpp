#include "../header/Camera.h"

Camera::Camera(uint64_t eyex, uint64_t eyey, uint64_t eyez, uint64_t centerx, uint64_t centery, uint64_t centerz, uint64_t upx, uint64_t upy, uint64_t upz, uint64_t fovy)
{
	pos = vec4(eyex, eyey, eyez, 1);
	center = vec4(centerx, centery, centerz, 1);
	up = vec4(upx, upy, upz, 1);
	fov = vec2(glm::radians((float)fovy), glm::radians((float)fovy));
}

void Camera::SetFOVx(uint64_t imageWidth, uint64_t imageHeight)
{
	fov.x = 2.0f * atan2(imageWidth * tan(fov.y / 2.0f), imageHeight);
}

void Camera::Transform()
{
	pos = transf * pos;
	up = transf * up;
}

mat4 Camera::LookAt()
{
	return glm::lookAt(vec3(pos), vec3(center), vec3(up));
}
