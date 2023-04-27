#include "../header/Scene.h"

void Scene::BuildAccelratedDatabase()
{
	std::vector<Intersectable> inter = std::vector<Intersectable>(meshes.size());
	std::transform(meshes.begin(), meshes.end(), inter.begin(), [](std::shared_ptr<Traceable> x) { return dynamic_cast<Intersectable*>(x.get()); });
	bvh.get()->QuickSetup(inter);
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
}
