#pragma once
#include <fstream>
#include <sstream>

#include "Scene.h"
#include "Timer.h"

class SceneBuilder 
{
public:
	virtual std::shared_ptr<Scene> BuildFromFile(const char* filename) = 0;
	RenderingOptions GetOptions() { return m_options; }
protected:
	RenderingOptions m_options;
};

class CustomSceneBuilder : public SceneBuilder
{
public:
	std::shared_ptr<Scene> BuildFromFile(const char* filename);
private:
	// Parse a general command's paramaters into 'parameters'.
	bool ReadCommandParameters(std::stringstream& cmdParams, const int numparams, float* parameters);
	// Intermediate storage for constructing meshes. List of pairs between vertex and vertex normal.
	std::vector<std::pair<vec4,vec3>> m_vertices;
	std::vector<Triangle> m_currentTriangles;
};