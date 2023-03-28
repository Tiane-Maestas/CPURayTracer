#include "../header/Scene.h"

void Scene::TransformObjects()
{
	for (auto& mesh : m_meshes) 
	{
		if (dynamic_cast<Ellipsoid*>(mesh.get())) continue; // Don't transform ellipsoids. Handled in renderer.
		mesh->Transform();
	}
	for (auto& light : m_lights)
	{
		if (dynamic_cast<Directional*>(light.get())) continue;
		light->Transform();
	}
	m_camera.Transform();
}
