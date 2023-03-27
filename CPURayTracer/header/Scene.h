#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Meshes.h"

class Scene {
public:
	Scene(){}
	~Scene(){}
	// Getters
	Camera getCamera() { return camera; }
	vector<shared_ptr<Light>> getLights() { return lights; }
	vector<shared_ptr<Mesh>> getMeshes() { return meshes; }
	// Setters
	void addCamera(Camera camera) { this->camera = camera; }
	void addMesh(shared_ptr<Mesh> mesh) { meshes.push_back(mesh); }
	void addLight(shared_ptr<Light> light) { lights.push_back(light); }
	// Update every object with their 'transf'.
	void TransformObjects();
private:
	Camera camera;
	vector<shared_ptr<Light>> lights;
	vector<shared_ptr<Mesh>> meshes;
};