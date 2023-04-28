#pragma once
#include "RayTracerCore.h"

class BoundingVolumeHierarchy
{
public:
	BoundingVolumeHierarchy() {}
	void QuickSetup(std::vector<Intersectable*> intersectables);
	Intersection Intersect(const Ray& ray) const;
private:
	// Total bounds of this root.
	aabb m_boundingBox = { vec3(FLT_MAX), vec3(-1 * FLT_MAX) };
	// Left and Right Subtrees
	std::shared_ptr<BoundingVolumeHierarchy> m_leftVolume = nullptr;
	std::shared_ptr<BoundingVolumeHierarchy> m_rightVolume = nullptr;

	Intersectable* m_leafVolume = nullptr;
};