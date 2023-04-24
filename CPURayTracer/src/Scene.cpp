#include "../header/Scene.h"

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
