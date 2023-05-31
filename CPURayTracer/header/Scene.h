#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Meshes.h"
#include "BoundingVolumes.h"
#include "Skybox.h"

struct Scene 
{
	// These are public since they will need to be accessed and modified frequently.
	Camera camera;
	EnviornmentMap::Skybox skybox;
	std::vector< std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Traceable>> meshes;
	// Acceleration Database of meshes.
	std::shared_ptr<BoundingVolumeHierarchy> bvh = std::make_shared<BoundingVolumeHierarchy>();
	void BuildAccelratedDatabase();
	// Update every object with their 'transf'.
	void TransformObjects();
};