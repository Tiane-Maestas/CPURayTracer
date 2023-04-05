#include "../header/Meshes.h"

Triangle::Triangle(vec4 a, vec4 b, vec4 c)
{
	m_a = a; m_b = b; m_c = c;
	m_normal = cross(vec3(b - a), vec3(c - a)); // Right hand rule definition.
	m_normal = normalize(m_normal);
}

void Triangle::Transform(const mat4& transf)
{
	m_a = transf * m_a; m_b = transf * m_b; m_c = transf * m_c;
	m_aNormal = transpose(inverse(mat3(transf))) * m_aNormal;
	m_bNormal = transpose(inverse(mat3(transf))) * m_bNormal;
	m_cNormal = transpose(inverse(mat3(transf))) * m_cNormal;
	m_normal = normalize(transpose(inverse(mat3(transf))) * m_normal);
}

vec3 Triangle::getNormal(vec4 pos)
{
	if (m_aNormal == vec3(0.0)) { // If 1 of the vertex normals are 0 then use flat normal.
		return m_normal;
	}
	// TODO: Add interpolation of normals for smooth shading.
	return vec3(0.0);
}

vec3 Mesh::getNormal(int id, vec4 pos)
{
	return triangles[id].getNormal(pos);
}

void Mesh::Transform()
{
	m_pos = m_transf * m_pos;
	for (Triangle& tri : triangles)
	{
		tri.Transform(m_transf);
	}
}

vec3 Sphere::getNormal(int id, vec4 pos)
{
	return normalize(pos - m_pos);
}

void Sphere::Transform()
{
	m_pos = m_transf * m_pos;
}

vec3 Ellipsoid::getNormal(int id, vec4 pos)
{
	pos = inverse(m_transf) * pos;
	vec3 norm = transpose(inverse(mat3(m_transf))) * (pos - m_pos);
	return normalize(norm);
}

void Ellipsoid::Transform()
{
}
