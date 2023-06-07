#include "../header/Scene.h"
#include "../header/Timer.h"

void Scene::BuildAccelratedDatabase()
{
	Timer bvhBuild("Build Scene BVH");
	std::vector<Intersectable*> intersectables = std::vector<Intersectable*>();
	for (std::shared_ptr<Traceable> mesh : meshes)
		intersectables.push_back(mesh.get());
	bvh.get()->QuickSetup(intersectables);
}

void Scene::TransformObjects()
{
	for (std::shared_ptr<Traceable> mesh : meshes) 
	{
		mesh->Transform();
	}
	for (std::shared_ptr<Light> light : lights)
	{
		light->Transform();
	}
	camera.Transform();

	// Must Re-Build the database anytime the scene is updated in case bounding boxes change.
	BuildAccelratedDatabase();
}
