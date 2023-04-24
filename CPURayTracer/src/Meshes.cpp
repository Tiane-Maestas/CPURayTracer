#include "../header/Meshes.h"

Triangle::Triangle(vec4 a, vec4 b, vec4 c)
{
	m_a = a; m_b = b; m_c = c;
	m_normal = cross(vec3(b - a), vec3(c - a)); // Right hand rule definition.
	m_normal = normalize(m_normal);
}

Intersection Triangle::Intersect(const Ray& ray)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumTriTests++;

	vec3 rayOrigin = ray.GetPosition();
	vec3 rayDir = ray.GetDirection();

	float denominator = dot(rayDir, m_normal);
	if (denominator != 0) // The ray hits the plane of the triangle if this isn't zero.
	{
		vec3 a(m_a); vec3 b(m_b); vec3 c(m_c);
		float t = (dot(a, m_normal) - dot(rayOrigin, m_normal)) / denominator;
		if (t > 0) // The triangle is in front of ray and not behind.
		{
			vec3 pos = ray.At(t);

			vec3 _a = cross(m_normal, c - b);
			vec3 A = _a / dot(_a, a - c);

			vec3 _b = cross(m_normal, a - c);
			vec3 B = _b / dot(_b, b - a);

			vec3 _c = cross(m_normal, b - a);
			vec3 C = _c / dot(_c, c - b);

			float alpha = dot(A, pos) - dot(A, c);
			float beta = dot(B, pos) - dot(B, a);
			float gamma = dot(C, pos) - dot(C, b);
			if (alpha >= 0 && beta >= 0 && gamma >= 0) // If true. Inside triangle.
			{
				return Intersection{ ray, ray.At(t), m_normal, vec2(0.0), {} };
			}
		}
	}

	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };
}

void Triangle::Transform(const mat4& transf)
{
	m_a = transf * m_a; m_b = transf * m_b; m_c = transf * m_c;
	m_aNormal = transpose(inverse(mat3(transf))) * m_aNormal;
	m_bNormal = transpose(inverse(mat3(transf))) * m_bNormal;
	m_cNormal = transpose(inverse(mat3(transf))) * m_cNormal;
	m_normal = normalize(transpose(inverse(mat3(transf))) * m_normal);
}

Intersection Mesh::Intersect(const Ray& ray)
{
	Intersection intersection = { ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), m_material };
	for (Triangle& tri : m_triangles) 
	{
		Intersection triIntersection = tri.Intersect(ray);
		if (length(triIntersection.hitPos - ray.GetPosition()) < length(intersection.hitPos - ray.GetPosition())) // Depth Test
			intersection = triIntersection;
	}
	intersection.material = m_material;
	return intersection;
}

void Mesh::Transform()
{
	m_pos = m_transf * m_pos;
	for (Triangle& tri : m_triangles)
	{
		tri.Transform(m_transf);
	}
}

Intersection Sphere::Intersect(const Ray& ray)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumSphTests++;

	vec3 rayOrigin = ray.GetPosition();
	vec3 rayDir = ray.GetDirection();
	vec3 pos(m_pos);

	float a = dot(rayDir, rayDir);
	float b = 2 * dot(rayDir, rayOrigin - pos);
	float c = dot(rayOrigin - pos, rayOrigin - pos) - pow(m_radius, 2);
	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0) // Hit Sphere
	{
		if (discriminant == 0) // Tangent to sphere.
		{
			float t = -0.5 * b / a;

			vec4 pos = ray.At(t);
			return Intersection{ ray, pos, normalize(pos - m_pos), vec2(0.0), m_material };
		}
		else
		{
			float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
			float t1 = q / a;
			float t2 = c / q;
			if (t1 > 0 && t2 > 0) // Hit sphere in two places.
			{
				t1 = std::min(t1, t2);

				vec4 pos = ray.At(t1);
				return Intersection{ ray, pos, normalize(pos - m_pos), vec2(0.0), m_material };
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0)
				{
					vec4 pos = ray.At(t2);
					return Intersection{ ray, pos, normalize(pos - m_pos), vec2(0.0), m_material };
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0)
				{
					vec4 pos = ray.At(t1);
					return Intersection{ ray, pos, normalize(pos - m_pos), vec2(0.0), m_material };
				}
			} // Two negative roots the sphere is behind.
		}
	}

	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), m_material };
}

void Sphere::Transform()
{
	m_pos = m_transf * m_pos;
}

Intersection Ellipsoid::Intersect(const Ray& ray)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumElpTests++;

	// Transform ray since ellipsoid's aren't transformed in scene.
	Ray rayCopy(ray.GetPosition(), ray.GetDirection());
	rayCopy.Transform(inverse(m_transf));

	vec3 rayOrigin = rayCopy.GetPosition();
	vec3 rayDir = rayCopy.GetDirection();
	vec3 pos(m_pos);

	float a = dot(rayDir, rayDir);
	float b = 2 * dot(rayDir, rayOrigin - pos);
	float c = dot(rayOrigin - pos, rayOrigin - pos) - pow(m_radius, 2);
	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0) // Hit Sphere
	{
		if (discriminant == 0) // Tangent to sphere.
		{
			float t = -0.5 * b / a;

			// Transform ray hit back to world coordintes but not normal.
			vec4 pos = rayCopy.At(t);
			return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), vec2(0.0), m_material };
		}
		else
		{
			float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
			float t1 = q / a;
			float t2 = c / q;
			if (t1 > 0 && t2 > 0) // Hit sphere in two places.
			{
				t1 = std::min(t1, t2);

				// Transform ray hit back to world coordintes but not normal.
				vec4 pos = rayCopy.At(t1);
				return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), vec2(0.0), m_material };
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0)
				{
					// Transform ray hit back to world coordintes but not normal.
					vec4 pos = rayCopy.At(t2);
					return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), vec2(0.0), m_material };
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0)
				{
					// Transform ray hit back to world coordintes but not normal.
					vec4 pos = rayCopy.At(t1);
					return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), vec2(0.0), m_material };
				}
			} // Two negative roots the sphere is behind.
		}
	}

	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), m_material };
}
