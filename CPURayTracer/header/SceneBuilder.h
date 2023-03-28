#pragma once
#include <fstream>
#include <sstream>

#include "Scene.h"

class SceneBuilder 
{
public:
	virtual Scene* BuildFromFile(const char* filename) = 0;
	RenderingOptions GetOptions() { return m_options; }
protected:
	RenderingOptions m_options;
};

class CustomSceneBuilder : public SceneBuilder
{
public:
	Scene* BuildFromFile(const char* filename);
private:
	// Parse a general command's paramaters into 'parameters'.
	bool ReadCommandParameters(stringstream& cmdParams, const int numparams, float* parameters);
	// Intermediate storage for constructing meshes. List of pairs between vertex and vertex normal.
	vector<pair<vec4,vec4>> m_vertices;
	vector<Triangle> m_triangles;
};