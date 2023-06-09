#pragma once
#include <fstream>
#include <sstream>

#include "Scene.h"
#include "Timer.h"

std::shared_ptr<TriangleMesh> parse_obj_file(std::string filepath);

class SceneBuilder 
{
public:
	virtual std::shared_ptr<Scene> BuildFromFile(const char* filename) = 0;
	RenderingOptions GetOptions() { return m_options; }
protected:
	RenderingOptions m_options;
};

class MyCustomSceneBuilder : public SceneBuilder
{
public:
	std::shared_ptr<Scene> BuildFromFile(const char* filename);
private:
	// Parse a general command's paramaters into 'parameters'.
	bool ReadCommandParameters(std::stringstream& cmdParams, const int numparams, float* parameters);
	// Intermediate storage for constructing meshes.
	TriangleMeshBuffers m_meshData;
	std::vector<ivec3> m_currentTriangleIndices;
};