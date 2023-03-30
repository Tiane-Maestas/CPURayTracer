#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Meshes.h"

class Scene {
public:
	Scene(){}
	~Scene(){}
	// These are public since they will need to be accessed and modified frequently.
	Camera camera;
	vector<shared_ptr<Light>> lights;
	vector<shared_ptr<Mesh>> meshes;
	// Update every object with their 'transf'.
	void TransformObjects();
};