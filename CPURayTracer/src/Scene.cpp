#include "../header/Scene.h"

void Scene::TransformObjects()
{
	for (auto& mesh : meshes) 
	{
		if (dynamic_cast<Ellipsoid*>(mesh.get())) continue; // Don't transform ellipsoids. Handled in renderer.
		mesh->Transform();
	}
	for (auto& light : lights) 
	{
		light->Transform();
	}
	camera.Transform();
}
