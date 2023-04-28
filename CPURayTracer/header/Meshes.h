#pragma once
#include "RayTracerCore.h"
#include "BoundingVolumes.h"
#include "Timer.h"

// You can intersect with a triangle but you cannot render a single triangle.
class Triangle : public Intersectable
{
public:
	Triangle(vec4 a, vec4 b, vec4 c);
	~Triangle() {}
	virtual Intersection Intersect(const Ray& ray) const;
	// Setters
	void SetNormals(vec3 aNormal, vec3 bNormal, vec3 cNormal) { m_aNormal = aNormal; m_bNormal = bNormal; m_cNormal = cNormal; }
	void Transform(const mat4& transf);
private:
	vec4 m_a = vec4(0.0), m_b = vec4(0.0), m_c = vec4(0.0); // Vertices
	vec3 m_aNormal = vec3(0.0), m_bNormal = vec3(0.0), m_cNormal = vec3(0.0); // Vertex Normals
	vec3 m_normal = vec3(0.0); // Plane Normal
	virtual void UpdateBounds();
};

class Mesh : public Traceable
{
public:
	Mesh(float x, float y, float z, std::string name) { m_name = name; m_pos = vec4(x, y, z, 1); }
	Mesh(vec4 pos, std::string name) { m_name = name; m_pos = pos; }
	virtual ~Mesh() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray) const;
	// This will transform mesh position and all triagnles by current 'm_transf'.
	virtual void Transform();
	// Setters
	void SetTriangles(std::vector<Triangle> triangles);
protected:
	std::vector<Triangle> m_triangles;
	BoundingVolumeHierarchy m_bvh = BoundingVolumeHierarchy();
	virtual void UpdateBounds();
};

// Primitives Below.
class Sphere : public Traceable
{
public:
	Sphere(float x, float y, float z, float r) { m_name = "Sphere"; m_pos = vec4(x, y, z, 1); m_radius = r; UpdateBounds(); }
	Sphere(vec4 pos, float r) { m_name = "Sphere"; m_pos = pos; m_radius = r; UpdateBounds(); }
	virtual ~Sphere() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray) const;
	virtual void Transform();
protected:
	float m_radius = 0;
	virtual void UpdateBounds();
};

class Ellipsoid : public Sphere
{
public:
	// An ellipsoid is a sphere that can be scaled in x, y, z.
	Ellipsoid(float x, float y, float z, float r) : Sphere(x, y, z, r) { m_name = "Ellipsoid"; }
	Ellipsoid(vec4 pos, float r) : Sphere(pos, r) { m_name = "Ellipsoid"; }
	~Ellipsoid() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray) const;
	virtual void Transform() { UpdateBounds(); } // Ellipsoids don't transform normally.
protected:
	virtual void UpdateBounds();
};