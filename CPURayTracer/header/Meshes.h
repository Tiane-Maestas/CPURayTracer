#pragma once
#include "RenderingPipelineConfig.h"
#include "Materials.h"

class Triangle 
{
public:
	Triangle(vec4 a, vec4 b, vec4 c);
	~Triangle() {}
	// Getters
	vec3 getNormal(vec4 pos);
	vec3 getFlatNormal() { return m_normal; }
	vec4 A() { return m_a; }
	vec4 B() { return m_b; }
	vec4 C() { return m_c; }
	// Setters
	void setNormals(vec3 aNormal, vec3 bNormal, vec3 cNormal) { m_aNormal = aNormal; m_bNormal = bNormal; m_cNormal = cNormal; }
	void Transform(const mat4& transf);
private:
	vec4 m_a = vec4(0.0), m_b = vec4(0.0), m_c = vec4(0.0); // Vertices
	vec3 m_aNormal = vec3(0.0), m_bNormal = vec3(0.0), m_cNormal = vec3(0.0); // Vertex Normals
	vec3 m_normal = vec3(0.0); // Plane Normal
};

class Mesh 
{
public:
	Mesh(float x, float y, float z, std::string name) { m_name = name; m_pos = vec4(x, y, z, 1); }
	Mesh(vec4 pos, std::string name) { m_name = name; m_pos = pos; }
	virtual ~Mesh() {}
	// Core Properties.
	Material material;
	std::vector<Triangle> triangles;
	// Getters
	std::string getName() { return m_name; }
	vec4 getPosition() { return m_pos; }
	mat4 getTransform() { return m_transf; }
	virtual vec3 getNormal(int id, vec4 pos); // Assumes position is passed in with world coordinates.
	// Setters
	void UpdateTransform(mat4 transf) { m_transf = transf; }
	// This will transform mesh position and all triagnles by current 'transf'.
	virtual void Transform();
protected:
	std::string m_name;
	vec4 m_pos = vec4(0.0);
	mat4 m_transf = mat4(1.0);
};

// Primitive Meshes Below.
class Sphere : public Mesh
{
public:
	Sphere(float x, float y, float z, float r) : Mesh(x, y, z, "Sphere") { m_radius = r; }
	Sphere(vec4 pos, float r) : Mesh(pos, "Sphere") { m_radius = r; }
	~Sphere(){}
	float getRadius() { return m_radius; }
	virtual vec3 getNormal(int id, vec4 pos);
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
	virtual vec3 getNormal(int id, vec4 pos);
	void Transform();
};