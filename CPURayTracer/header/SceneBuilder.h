#pragma once
#include <fstream>
#include <sstream>

#include "Scene.h"

class SceneBuilder 
{
public:
	virtual Scene* buildFromFile(const char* filename) = 0;
	RenderingOptions GetOptions() { return options; }
protected:
	RenderingOptions options;
};

class CustomSceneBuilder : public SceneBuilder
{
public:
	Scene* buildFromFile(const char* filename);
private:
	// Parse a general command's paramaters into 'parameters'.
	bool readCommandParameters(stringstream& cmdParams, const int numparams, float* parameters);
	// Intermediate storage for constructing meshes. List of pairs between vertex and vertex normal.
	vector<pair<vec4,vec4>> vertices;
	vector<Triangle> triangles;
};