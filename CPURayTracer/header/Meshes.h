#pragma once
#include "RayTracerCore.h"

class Triangle 
{
public:
	Triangle(vec4 a, vec4 b, vec4 c);
	~Triangle() {}
	Intersection Intersect(const Ray& ray);
	// Setters
	void SetNormals(vec3 aNormal, vec3 bNormal, vec3 cNormal) { m_aNormal = aNormal; m_bNormal = bNormal; m_cNormal = cNormal; }
	void Transform(const mat4& transf);
private:
	vec4 m_a = vec4(0.0), m_b = vec4(0.0), m_c = vec4(0.0); // Vertices
	vec3 m_aNormal = vec3(0.0), m_bNormal = vec3(0.0), m_cNormal = vec3(0.0); // Vertex Normals
	vec3 m_normal = vec3(0.0); // Plane Normal
};

class Mesh : public Traceable
{
public:
	Mesh(float x, float y, float z, std::string name) { m_name = name; m_pos = vec4(x, y, z, 1); }
	Mesh(vec4 pos, std::string name) { m_name = name; m_pos = pos; }
	virtual ~Mesh() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray);
	// This will transform mesh position and all triagnles by current 'm_transf'.
	virtual void Transform();
	// Setters
	void SetTriangles(std::vector<Triangle> triangles) { m_triangles = triangles; }
protected:
	std::vector<Triangle> m_triangles;
};

// Primitives Below.
class Sphere : public Traceable
{
public:
	Sphere(float x, float y, float z, float r) { m_name = "Sphere"; m_pos = vec4(x, y, z, 1); m_radius = r; }
	Sphere(vec4 pos, float r) { m_name = "Sphere"; m_pos = pos; m_radius = r; }
	virtual ~Sphere() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray);
	virtual void Transform();
protected:
	float m_radius = 0;
};

class Ellipsoid : public Sphere
{
public:
	// An ellipsoid is a sphere that can be scaled in x, y, z.
	Ellipsoid(float x, float y, float z, float r) : Sphere(x, y, z, r) { m_name = "Ellipsoid"; }
	Ellipsoid(vec4 pos, float r) : Sphere(pos, r) { m_name = "Ellipsoid"; }
	~Ellipsoid() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray);
	virtual void Transform() {} // Ellipsoids don't transform normally.
};