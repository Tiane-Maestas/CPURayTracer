#include "../header/Meshes.h"

Triangle::Triangle(int a, int b, int c, TriangleMeshBuffers* buffers)
{
	m_indexA = a; m_indexB = b; m_indexC = c;
	m_buffers = buffers;
	m_normal = cross(vec3(m_buffers->positions[m_indexB] - m_buffers->positions[m_indexA]), vec3(m_buffers->positions[m_indexC] - m_buffers->positions[m_indexA])); // Right hand rule definition.
	m_normal = normalize(m_normal);
	UpdateBounds();
}

void Triangle::UpdateBounds() 
{
	for (int xyz = 0; xyz < 3; xyz++)
	{
		float minVal = m_buffers->positions[m_indexA][xyz]; // Vertex 1
		float maxVal = m_buffers->positions[m_indexA][xyz]; // Vertex 1

		minVal = std::min(minVal, m_buffers->positions[m_indexB][xyz]); // Vertex 2
		maxVal = std::max(maxVal, m_buffers->positions[m_indexB][xyz]); // Vertex 2

		minVal = std::min(minVal, m_buffers->positions[m_indexC][xyz]); // Vertex 3
		maxVal = std::max(maxVal, m_buffers->positions[m_indexC][xyz]); // Vertex 3

		m_boundingBox.minimums[xyz] = minVal;
		m_boundingBox.maximums[xyz] = maxVal;
	}
}

Intersection Triangle::Intersect(const Ray& ray) const
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumTriTests++;

	vec3 rayOrigin = ray.GetPosition();
	vec3 rayDir = ray.GetDirection();

	vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = m_buffers->positions[m_indexB] - m_buffers->positions[m_indexA];
	edge2 = m_buffers->positions[m_indexC] - m_buffers->positions[m_indexA];
	h = cross(rayDir, edge2);
	a = dot(edge1, h);

	if (a == 0.0f)
		return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };    // This ray is parallel to this triangle.

	f = 1.0f / a;
	s = rayOrigin - vec3(m_buffers->positions[m_indexA]);
	u = f * dot(s, h);

	if (u < 0.0f || u > 1.0f)
		return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };

	q = cross(s, edge1);
	v = f * dot(rayDir, q);

	if (v < 0.0f || u + v > 1.0f)
		return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };

	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);

	if (t > 0.0f) // ray intersection
	{
		Intersection intersection = { ray, ray.At(t), m_normal, vec2(u, v), {} };
		// Get Normals if user defined.
		if (m_buffers->normals.size() == m_buffers->positions.size() && !FlatShadingOnly)
		{
			vec3 n0 = m_buffers->normals[m_indexA];
			vec3 n1 = m_buffers->normals[m_indexB];
			vec3 n2 = m_buffers->normals[m_indexC];
			float w1 = u; float w2 = v; // Normal weights for interpolation.
			intersection.normal = w1 * n1 + w2 * n2 + n0 * (1.0f - w1 - w2);
		}
		// Get UVs if user defined.
		if (m_buffers->uvs.size() == m_buffers->positions.size())
		{
			vec2 uv0 = m_buffers->uvs[m_indexA];
			vec2 uv1 = m_buffers->uvs[m_indexB];
			vec2 uv2 = m_buffers->uvs[m_indexC];
			float w1 = u; float w2 = v; // UV weights for interpolation.
			intersection.uv = w1 * uv1 + w2 * uv2 + uv0 * (1.0f - w1 - w2);
		}
		return intersection;
	}

	// This means that there is a line intersection but not a ray intersection.
	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };
}

void Triangle::Transform(const mat4& transf)
{
	m_normal = normalize(transpose(inverse(mat3(transf))) * m_normal);
	UpdateBounds();
}

Intersection TriangleMesh::Intersect(const Ray& ray) const
{
	Intersection intersection = { ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), &m_material };
	if (!UseBVHAcceleration)
	{
		for (const Triangle& tri : m_triangles)
		{
			Intersection triIntersection = tri.Intersect(ray);
			if (length(triIntersection.hitPos - ray.GetPosition()) < length(intersection.hitPos - ray.GetPosition())) // Depth Test
				intersection = triIntersection;
		}		
	}
	else 
	{
		intersection = m_bvh.Intersect(ray);
		intersection.material = &m_material; // Triangles don't have access to the mateiral.
	}

	return intersection;
}

void TriangleMesh::Transform()
{
	// Transform Vertices.
	m_pos = m_transf * m_pos;
	for(vec4& pos : m_buffers.positions)
		pos = m_transf * pos;

	// Transform Normals.
	for (vec3& norm : m_buffers.normals)
		norm = normalize(transpose(inverse(mat3(m_transf))) * norm);

	for (Triangle& tri : m_triangles)
		tri.Transform(m_transf);

	UpdateBounds();
}

void TriangleMesh::SetTriangles(std::vector<ivec3> indices)
{
	for (ivec3 triIndices : indices)
		m_triangles.push_back(Triangle(triIndices.x, triIndices.y, triIndices.z, &m_buffers));

	UpdateBounds();
}

void TriangleMesh::UpdateBounds()
{
	for (Triangle& tri : m_triangles)
	{
		aabb currBB = tri.GetBounds();

		for (int xyz = 0; xyz < 3; xyz++)
		{
			float minVal = currBB.minimums[xyz];
			float maxVal = currBB.maximums[xyz];

			minVal = std::min(minVal, m_boundingBox.minimums[xyz]);
			maxVal = std::max(maxVal, m_boundingBox.maximums[xyz]);

			m_boundingBox.minimums[xyz] = minVal;
			m_boundingBox.maximums[xyz] = maxVal;
		}
	}

	// Anytime bounds are updated you have to Re-Build the BVH.
	Timer bvhBuild("Build/Rebuild Mesh " + m_name + " BVH");
	std::vector<Intersectable*> intersectables = std::vector<Intersectable*>();
	for (Triangle& tri : m_triangles)
		intersectables.push_back(&tri);
	m_bvh.QuickSetup(intersectables);
}

// Helper for calculating sphere UV
vec2 GetSphereUV(const vec3& normal) 
{
	double theta = acos(1.0 * normal.y);
	double phi = atan2(-1.0 * normal.z, normal.x) + pi;
	return vec2(phi / (2.0 * pi), theta / pi);
}

Intersection Sphere::Intersect(const Ray& ray) const
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
			return Intersection{ ray, pos, normalize(pos - m_pos), GetSphereUV(normalize(pos - m_pos)), &m_material};
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
				return Intersection{ ray, pos, normalize(pos - m_pos), GetSphereUV(normalize(pos - m_pos)), &m_material };
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0)
				{
					vec4 pos = ray.At(t2);
					return Intersection{ ray, pos, normalize(pos - m_pos), GetSphereUV(normalize(pos - m_pos)), &m_material };
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0)
				{
					vec4 pos = ray.At(t1);
					return Intersection{ ray, pos, normalize(pos - m_pos), GetSphereUV(normalize(pos - m_pos)), &m_material };
				}
			} // Two negative roots the sphere is behind.
		}
	}

	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), &m_material };
}

void Sphere::Transform()
{
	m_pos = m_transf * m_pos;
	UpdateBounds();
}

void Sphere::UpdateBounds()
{
	m_boundingBox.minimums = m_pos - m_radius;
	m_boundingBox.maximums = m_pos + m_radius;
}

Intersection Ellipsoid::Intersect(const Ray& ray) const
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
			return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), GetSphereUV(normalize(pos - m_pos)), &m_material };
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
				return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), GetSphereUV(normalize(pos - m_pos)), &m_material };
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0)
				{
					// Transform ray hit back to world coordintes but not normal.
					vec4 pos = rayCopy.At(t2);
					return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), GetSphereUV(normalize(pos - m_pos)), &m_material };
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0)
				{
					// Transform ray hit back to world coordintes but not normal.
					vec4 pos = rayCopy.At(t1);
					return Intersection{ ray, (m_transf * pos), normalize(transpose(inverse(mat3(m_transf))) * (pos - m_pos)), GetSphereUV(normalize(pos - m_pos)), &m_material };
				}
			} // Two negative roots the sphere is behind.
		}
	}

	return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), &m_material };
}

void Ellipsoid::UpdateBounds()
{
	m_boundingBox.minimums = m_pos - m_radius;
	m_boundingBox.maximums = m_pos + m_radius;
	m_boundingBox.minimums = m_transf * vec4(m_boundingBox.minimums, 1);
	m_boundingBox.maximums = m_transf * vec4(m_boundingBox.maximums, 1);
}
