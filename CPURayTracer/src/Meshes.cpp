#include "../header/Meshes.h"

Triangle::Triangle(vec4 a, vec4 b, vec4 c)
{
	m_a = a; m_b = b; m_c = c;
	m_normal = vec4(cross(vec3(b - a), vec3(c - a)), 0); // Right hand rule definition.
	m_normal = normalize(m_normal);
}

void Triangle::Transform(const mat4& transf)
{
	m_a = transf * m_a; m_b = transf * m_b; m_c = transf * m_c;
	m_aNorm = transpose(inverse(transf)) * m_aNorm;
	m_bNorm = transpose(inverse(transf)) * m_bNorm;
	m_cNorm = transpose(inverse(transf)) * m_cNorm;
	m_normal = normalize(transpose(inverse(transf)) * m_normal);
	m_normal = vec4(cross(vec3(m_b - m_a), vec3(m_c - m_a)), 0); // Right hand rule definition.
	m_normal = normalize(m_normal);
}

vec4 Triangle::getNormal(vec4 pos)
{
	if (m_aNorm == vec4(0.0)) { // If 1 of the vertex normals are 0 then use flat normal.
		return m_normal;
	}
	// TODO: Add interpolation of normals for smooth shading.
	return vec4(0.0);
}

vec4 Mesh::getNormal(int id, vec4 pos)
{
	return m_triangles[id].getNormal(pos);
}

void Mesh::Transform()
{
	m_pos = m_transf * m_pos;
	for (Triangle& tri : m_triangles)
	{
		tri.Transform(m_transf);
	}
}

vec4 Sphere::getNormal(int id, vec4 pos)
{
	return normalize(pos - m_pos);
}

void Sphere::Transform()
{
	m_pos = m_transf * m_pos;
}

vec4 Ellipsoid::getNormal(int id, vec4 pos)
{
	pos = inverse(m_transf) * pos;
	vec4 norm = transpose(inverse(m_transf)) * (pos - m_pos);
	norm.w = 0; // Need homogenous coord. to be 0 or else normalize won't by 1 for vec3 in color.
	return normalize(norm);
}

void Ellipsoid::Transform()
{
}
