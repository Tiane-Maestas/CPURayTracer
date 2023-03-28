#pragma once
#include "RenderingPipelineConfig.h"
#include "Materials.h"

class Triangle 
{
public:
	Triangle(vec4 a, vec4 b, vec4 c);
	~Triangle() {}
	// Getters
	vec4 getNormal(vec4 pos);
	vec4 getFlatNormal() { return m_normal; }
	vec4 A() { return m_a; }
	vec4 B() { return m_b; }
	vec4 C() { return m_c; }
	// Setters
	void setNormals(vec4 aNorm, vec4 bNorm, vec4 cNorm) { m_aNorm = aNorm; m_bNorm = bNorm; m_cNorm = cNorm; }
	void Transform(const mat4& transf);
private:
	vec4 m_a = vec4(0.0), m_b = vec4(0.0), m_c = vec4(0.0); // Vertices
	vec4 m_aNorm = vec4(0.0), m_bNorm = vec4(0.0), m_cNorm = vec4(0.0); // Normals
	vec4 m_normal = vec4(0.0); // Plane Normal
};

class Mesh 
{
public:
	Mesh(float x, float y, float z, string name) { m_name = name; m_pos = vec4(x, y, z, 1); }
	Mesh(vec4 pos, string name) { m_name = name; m_pos = pos; }
	virtual ~Mesh() {}
	// Getters
	vec4 getPosition() { return m_pos; }
	mat4 getTransform() { return m_transf; }
	Material getMaterial() { return m_material; }
	virtual vec4 getNormal(int id, vec4 pos); // Assumes position is passed in with world coordinates.
	vector<Triangle> getTriangles() { return m_triangles; }
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	void setMaterial(Material material) { m_material = material; }
	void addTriangles(vector<Triangle> triangles) { m_triangles = triangles; }
	// This will transform mesh position and all triagnles by current 'transf'.
	virtual void Transform();
protected:
	string m_name;
	vec4 m_pos = vec4(0.0);
	mat4 m_transf = mat4(1.0);
	Material m_material;
	vector<Triangle> m_triangles;
};

// Primitive Meshes Below.
class Sphere : public Mesh
{
public:
	Sphere(float x, float y, float z, float r) : Mesh(x, y, z, "Sphere") { m_radius = r; }
	Sphere(vec4 pos, float r) : Mesh(pos, "Sphere") { m_radius = r; }
	~Sphere(){}
	float getRadius() { return m_radius; }
	vec4 getNormal(int id, vec4 pos);
	void Transform();
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
	vec4 getNormal(int id, vec4 pos);
	void Transform();
};