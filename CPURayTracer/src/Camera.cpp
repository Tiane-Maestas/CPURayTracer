#include "../header/Camera.h"

Camera::Camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz, uint32_t fovy)
{
	m_pos = vec4(eyex, eyey, eyez, 1);
	m_center = vec4(centerx, centery, centerz, 1);
	m_up = vec4(upx, upy, upz, 1);
	m_fov = vec2(glm::radians((float)fovy), glm::radians((float)fovy));
}

void Camera::setFOVx(float imageWidth, float imageHeight)
{
	m_fov.x = 2.0f * atan2(imageWidth * tan(m_fov.y / 2.0f), imageHeight);
}

void Camera::Transform()
{
	m_pos = m_transf * m_pos;
	m_up = mat3(m_transf) * m_up; // May want to transform this like a normal.
}

mat4 Camera::LookAt()
{
	return glm::lookAt(vec3(m_pos), vec3(m_center), m_up);
}
