#include "../header/RayTracerCore.h"

vec4 Ray::At(float t)
{
	if (t == FLT_MAX) { return vec4(FLT_MAX); }
	return m_pos + t * m_dir;
}

void Ray::Transform(const mat4& transf)
{
	m_pos = transf * m_pos;
	m_dir = transf * m_dir;
}