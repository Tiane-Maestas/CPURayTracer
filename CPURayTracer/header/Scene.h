#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Meshes.h"

class Scene {
public:
	Scene(){}
	~Scene(){}
	// Getters
	Camera* getCamera() { return &m_camera; }
	vector<shared_ptr<Light>>* getLights() { return &m_lights; }
	vector<shared_ptr<Mesh>>* getMeshes() { return &m_meshes; }
	// Setters
	void addCamera(Camera camera) { m_camera = camera; }
	void addMesh(shared_ptr<Mesh> mesh) { m_meshes.push_back(mesh); }
	void addLight(shared_ptr<Light> light) { m_lights.push_back(light); }
	// Update every object with their 'transf'.
	void TransformObjects();
private:
	Camera m_camera;
	vector<shared_ptr<Light>> m_lights;
	vector<shared_ptr<Mesh>> m_meshes;
};