#include "../header/BoundingVolumes.h"

// Comparator for sorting BoundingVolumeHierarchy intersectables list.
inline bool box_compare(const Intersectable& intersectable_a, const Intersectable& intersectable_b, int axis) {
    return intersectable_a.GetBounds().minimums[axis] < intersectable_b.GetBounds().minimums[axis];
}

bool box_x_compare(const Intersectable& intersectable_a, const Intersectable& intersectable_b) {
    return box_compare(intersectable_a, intersectable_b, 0);
}

bool box_y_compare(const Intersectable& intersectable_a, const Intersectable& intersectable_b) {
    return box_compare(intersectable_a, intersectable_b, 1);
}

bool box_z_compare(const Intersectable& intersectable_a, const Intersectable& intersectable_b) {
    return box_compare(intersectable_a, intersectable_b, 2);
}

void BoundingVolumeHierarchy::QuickSetup(std::vector<Intersectable> intersectables)
{
    // Exit Recursion
    if (intersectables.size() == 1)
        m_leafVolume = &intersectables[0]; return;

    // Set bounding box of current node considering it contains all of the current intersectables.
    for (Intersectable& intersectable : intersectables)
    {
        aabb currBB = intersectable.GetBounds();

        for (int xyz = 0; xyz < 3; xyz++)
        {
            float minVal = currBB.minimums[xyz];
            float maxVal = currBB.maximums[xyz];

            minVal = std::min(minVal, m_boundingBox.minimums[xyz]);
            maxVal = std::max(maxVal, m_boundingBox.maximums[xyz]);

            m_boundingBox.minimums[xyz] = minVal;
            m_boundingBox.maximums[xyz] = maxVal;
        }
    }

     // Split intersectables by axis with greatest range in bounding box. 
    float maxRange = std::max(m_boundingBox.maximums[0] - m_boundingBox.minimums[0], m_boundingBox.maximums[1] - m_boundingBox.minimums[1]);
    maxRange = std::max(maxRange, m_boundingBox.maximums[2] - m_boundingBox.minimums[2]);

    if (maxRange == (m_boundingBox.maximums[0] - m_boundingBox.minimums[0]))
        std::sort(intersectables.begin(), intersectables.end(), box_x_compare);
    else if (maxRange == (m_boundingBox.maximums[1] - m_boundingBox.minimums[1]))
        std::sort(intersectables.begin(), intersectables.end(), box_y_compare);
    else
        std::sort(intersectables.begin(), intersectables.end(), box_z_compare);

    m_leftVolume = std::make_shared<BoundingVolumeHierarchy>();
    m_rightVolume = std::make_shared<BoundingVolumeHierarchy>();
    m_leftVolume.get()->QuickSetup(std::vector<Intersectable>(intersectables.begin(), intersectables.begin() + intersectables.size() / 2));
    m_rightVolume.get()->QuickSetup(std::vector<Intersectable>(intersectables.begin() + intersectables.size() / 2, intersectables.end()));
}

Intersection BoundingVolumeHierarchy::Intersect(const Ray& ray) const
{
    // If leaf intersect with Intersectable.
    if (m_leafVolume != nullptr)
        return m_leafVolume->Intersect(ray);

    // If you didn't hit this bounding box you didn't hit any of its children.
    if (!m_boundingBox.Intersect(ray))
        return Intersection{ ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };

    Intersection leftIntersection = m_leftVolume.get()->Intersect(ray);
    Intersection rightIntersection = m_rightVolume.get()->Intersect(ray);

    if (length(leftIntersection.hitPos - ray.GetPosition()) < length(rightIntersection.hitPos - ray.GetPosition())) // Depth Test
        return leftIntersection;
    
    return rightIntersection;
}
