#pragma once
#include "RenderingPipelineConfig.h"
#include "Materials.h"

class Triangle 
{
public:
	Triangle(vec4 a, vec4 b, vec4 c);
	~Triangle() {}
	void setNormals(vec4 aNorm, vec4 bNorm, vec4 cNorm) { this->aNorm = aNorm; this->bNorm = bNorm; this->cNorm = cNorm; }
	void Transform(const mat4& transf);
	vec4 getNormal(vec4 pos);
private:
	vec4 a = vec4(0.0), b = vec4(0.0), c = vec4(0.0); // Vertices
	vec4 aNorm = vec4(0.0), bNorm = vec4(0.0), cNorm = vec4(0.0); // Normals
	vec4 normal = vec4(0.0); // Plane Normal
};

class Mesh 
{
public:
	Mesh(uint64_t x, uint64_t y, uint64_t z, string name) { this->name = name; pos = vec4(x, y, z, 1); }
	Mesh(vec4 pos, string name) { this->name = name; this->pos = pos; }
	virtual ~Mesh() {}
	// Getters
	virtual vec4 getNormal(int id, vec4 pos); // Assumes position is passed in with world coordinates.
	// Setters
	void UpdateTransform(mat4 transf) { this->transf = transf; }
	void setMaterial(Material material) { this->material = material; }
	void addTriangles(vector<Triangle> tris) { triangles = tris; }
	// This will transform mesh position and all triagnles by current 'transf'.
	virtual void Transform();
protected:
	string name;
	vec4 pos = vec4(0.0);
	mat4 transf = mat4(1.0);
	Material material;
	vector<Triangle> triangles;
};

// Primitive Meshes Below.
class Sphere : public Mesh
{
public:
	Sphere(uint64_t x, uint64_t y, uint64_t z, uint64_t r) : Mesh(x, y, z, "Sphere") { radius = r; }
	Sphere(vec4 pos, uint64_t r) : Mesh(pos, "Sphere") { radius = r; }
	~Sphere(){}
	vec4 getNormal(vec4 pos);
	void Transform();
protected:
	uint64_t radius = 0;
};

class Ellipsoid : public Sphere
{
public:
	// An ellipsoid is a sphere that can be scaled in x, y, z.
	Ellipsoid(uint64_t x, uint64_t y, uint64_t z, uint64_t r) : Sphere(x, y, z, r) { name = "Ellipsoid"; }
	Ellipsoid(vec4 pos, uint64_t r) : Sphere(pos, r) { name = "Ellipsoid"; }
	~Ellipsoid() {}
	vec4 getNormal(vec4 pos);
	void Transform();
};