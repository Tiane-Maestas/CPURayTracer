#include "../header/RayTracerCore.h"

vec4 Ray::At(float t) const
{
	if (t == FLT_MAX) { return vec4(FLT_MAX); }
	return m_pos + t * m_dir;
}

void Ray::Transform(const mat4& transf)
{
	m_pos = transf * m_pos;
	m_dir = transf * m_dir;
}

bool aabb::Intersect(const Ray& ray) const
{
    float t_min = -1.0 * FLT_MAX;
    float t_max = FLT_MAX;
    for (int xyz = 0; xyz < 3; xyz++)
    {
        float invDenom = 1.0 / ray.GetDirection()[xyz];
        float t0 = (minimums[xyz] - ray.GetPosition()[xyz]) * invDenom;
        float t1 = (maximums[xyz] - ray.GetPosition()[xyz]) * invDenom;
        if (invDenom < 0.0f)
            std::swap(t0, t1);
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max < t_min) // Overlapping Bounds in an axis. (Not <= in case intersect with other axes).
            return false;
    }
    return true;
}