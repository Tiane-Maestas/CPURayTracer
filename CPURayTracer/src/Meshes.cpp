#include "../header/Meshes.h"

Triangle::Triangle(vec4 a, vec4 b, vec4 c)
{
	this->a = a; this->b = b; this->c = c;
	normal = vec4(cross(vec3(b) - vec3(a), vec3(c) - vec3(a)), 1); // Right hand rule definition.
	normal = normalize(normal);
}

void Triangle::Transform(const mat4& transf)
{
	a = transf * a; b = transf * b; c = transf * c;
	aNorm = transpose(inverse(transf)) * aNorm;
	bNorm = transpose(inverse(transf)) * bNorm;
	cNorm = transpose(inverse(transf)) * cNorm;
	normal = transpose(inverse(transf)) * normal;
}

vec4 Triangle::getNormal(vec4 pos)
{
	if (aNorm == vec4(0.0)) { // If 1 of the vertex normals are 0 then use flat normal.
		return normal;
	}
	// TODO: Add interpolation of normals for smooth shading.
	return vec4(0.0);
}

vec4 Mesh::getNormal(int id, vec4 pos)
{
	return triangles[id].getNormal(pos);
}

void Mesh::Transform()
{
	pos = transf * pos;
	for (Triangle tri : triangles) 
	{
		tri.Transform(transf);
	}
}

vec4 Sphere::getNormal(vec4 pos)
{
	return normalize(pos - this->pos);
}

void Sphere::Transform()
{
	pos = transf * pos;
}

vec4 Ellipsoid::getNormal(vec4 pos)
{
	pos = inverse(transf) * pos;
	return transpose(inverse(transf)) * normalize(pos - this->pos);
}

void Ellipsoid::Transform()
{
}
