#include "../header/RayTracer.h"

vec4 Ray::At(float t)
{
	if (t == FLT_MAX) { return vec4(FLT_MAX); }
	return m_pos + t * m_dir;
}

void Ray::transform(mat4 transf)
{
	m_pos = transf * m_pos;
	m_dir = transf * m_dir;
}

RayTracer::RayTracer(float width, float height)
{
	m_imagePixels = new uint8_t[3 * (width * height)];
	m_imageWidth = width;
	m_imageHeight = height;
}

RayTracer::~RayTracer()
{
	delete m_imagePixels;
}
