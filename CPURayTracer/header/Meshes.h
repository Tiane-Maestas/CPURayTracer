#pragma once
#include "RayTracerCore.h"
#include "BoundingVolumes.h"
#include "Timer.h"

// This groups all of the necessary buffers for triangle vertex information.
struct TriangleMeshBuffers
{
	std::vector<vec4> positions;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;
};

// You can intersect with a triangle but you cannot render a single triangle.
class Triangle : public Intersectable
{
public:
	Triangle(int a, int b, int c, TriangleMeshBuffers* buffers);
	~Triangle() {}
	virtual Intersection Intersect(const Ray& ray) const;
	// Setters
	void Transform(const mat4& transf);
private:
	int m_indexA, m_indexB, m_indexC; // Indices of vertex information stored in mesh buffers.
	TriangleMeshBuffers* m_buffers = nullptr;
	vec3 m_normal = vec3(0.0); // Plane Normal
	virtual void UpdateBounds();
};

// You can render complete triangle mesh.
class TriangleMesh : public Traceable
{
public:
	TriangleMesh(TriangleMeshBuffers buffers, std::string name) { m_buffers = buffers; m_name = name; m_pos = buffers.positions[0]; }
	virtual ~TriangleMesh() {}
	// Traceable	
	virtual Intersection Intersect(const Ray& ray) const;
	// This will transform mesh position and all triagnles by current 'm_transf'.
	virtual void Transform();
	// Setters
	void SetTriangles(std::vector<ivec3> indices);
protected:
	TriangleMeshBuffers m_buffers;
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