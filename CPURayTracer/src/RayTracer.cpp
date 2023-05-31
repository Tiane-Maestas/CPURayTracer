#include "../header/RayTracer.h"

RayTracer::RayTracer(Image* imagePixels, float width, float height)
{
	m_imagePixels = imagePixels;
	m_imageWidth = width;
	m_imageHeight = height;
}

// ---Main Logic Loop---
void RayTracer::TraceImage(Scene* scene, const PixelBlock& block)
{
	int pixel = 3 * (block.xMin + (block.yMin * m_imageWidth));
	for (int y = block.yMin; y < block.yMax; y++)
	{
		for (int x = block.xMin; x < block.xMax; x++)
		{
			Ray ray = RayThroughPixel(scene, 0.5f + x, 0.5f + y);
			Intersection intersection = FindIntersection(scene, ray);
			vec3 col = FindColor(scene, intersection, 0);
			(*m_imagePixels)(x, y) = col;
		}
	}
}

void RayTracer::TraceImage(Scene* scene, const std::vector<PixelBlock>& blocks)
{
	m_thread = std::make_shared<std::thread>(&RayTracer::TraceThreadedImage, this, scene, blocks);
}

void RayTracer::TraceThreadedImage(Scene* scene, const std::vector<PixelBlock>& blocks)
{
	for (const PixelBlock& block : blocks) 
	{
		TraceImage(scene, block);
		if (ProgressReport::ReportProgress) 
		{
			ProgressReport::RenderedBlocks++;
			ProgressReport::Print();
		}
	}
}

Ray RayTracer::RayThroughPixel(Scene* scene, float x, float y)
{
	float a = x - (m_imageWidth / 2);
	float b = y - (m_imageHeight / 2);
	float alpha = tan(scene->camera.GetFOV().x / 2) * (a / (m_imageWidth / 2));
	float beta = tan(scene->camera.GetFOV().y / 2) * (b / (m_imageHeight / 2));
	vec3 w = normalize(scene->camera.GetPosition() - scene->camera.GetLookingAt());
	vec3 u = normalize(cross(vec3(scene->camera.GetUp()), w));
	vec3 v = normalize(cross(w, u));

	vec3 dir = alpha * u + beta * v - w;
	dir = normalize(dir);

	return Ray(scene->camera.GetPosition(), dir);
}

Intersection RayTracer::FindIntersection(Scene* scene, const Ray& ray)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::RaysCasted++;

	if (!UseBVHAcceleration) 
	{
		Intersection intersection = { ray, vec4(FLT_MAX), vec3(0.0), vec2(0.0), {} };
		for (std::shared_ptr<Traceable> mesh : scene->meshes)
		{
			Intersection meshIntersection = mesh->Intersect(ray);
			if (length(meshIntersection.hitPos - ray.GetPosition()) < length(intersection.hitPos - ray.GetPosition())) // Depth Test
				intersection = meshIntersection;
		}
		return intersection;
	}

	return scene->bvh.get()->Intersect(ray);
}

// Helper function for 'FindColor'
vec3 ComputeColor(const vec3& direction, const vec3& normal, const vec3& halfvec, const vec3& color, const vec3& diffuse, const vec3& specular, const float shininess, const vec3& attenuation, const float intensity, const float distance)
{
	float nDotL = dot(normal, direction);
	vec3 lambert = diffuse * color * std::max(nDotL, 0.0f);

	float nDotH = dot(normal, halfvec);
	vec3 phong = specular * color * pow(std::max(nDotH, 0.0f), shininess);

	float attenuate = intensity / (attenuation.x + attenuation.y * distance + attenuation.z * pow(distance, 2));

	return attenuate * (lambert + phong);
}

vec3 RayTracer::FindColor(Scene* scene, const Intersection& intersection, int recursiveCall)
{
	if (intersection.hitPos == vec4(FLT_MAX)) { return (m_useSkyBox) ? scene->skybox.Query(intersection.ray.GetDirection()) : m_defaultColor; } // Return background if hit nothing.

	// Add color from perfect mirror reflections.
	if (intersection.material.type == MaterialType::Mirror)
	{
		vec3 hitDir = intersection.ray.GetDirection();
		vec4 direction = vec4(normalize(hitDir - 2 * dot(hitDir, intersection.normal) * intersection.normal), 0);
		Ray reflectionRay(intersection.hitPos + 0.01f * direction, direction);
		return ColorFromReflections(scene, reflectionRay, recursiveCall);
	}

	vec3 currentColor = intersection.material.ambient + intersection.material.emission;

	// Add color from the lights.
	vec4 towardsRayOrigin = normalize(intersection.ray.GetPosition() - intersection.hitPos); // Vector to origin of ray cast. Needed for half vector.
	for (std::shared_ptr<Light> light : scene->lights)
	{
		if (Directional* directional = dynamic_cast<Directional*>(light.get()))
		{
			vec4 direction = normalize(directional->GetPosition());
			
			// Don't add light color if there is an object blocking it's view of the light. (Shadows)
			Ray shadowRay(intersection.hitPos + 0.001f * direction, direction); // Account for shadow acne.
			Intersection shadowIntersection = FindIntersection(scene, shadowRay);
			if (shadowIntersection.hitPos == vec4(FLT_MAX)) // Direction lights only have one check becuase they have no position.
			{
				vec4 half = normalize(direction + towardsRayOrigin); // The vector halfway between the ray origin and light.
				currentColor += ComputeColor(direction, intersection.normal, half, directional->color, intersection.material.diffuse, intersection.material.specular, intersection.material.shininess, Directional::attenuation, 1.0f, 0.0f);
			}
		}
		else if (Point* point = dynamic_cast<Point*>(light.get()))
		{
			vec4 toLight = point->GetPosition() - intersection.hitPos;
			vec4 direction = normalize(toLight); // Direction to light.
			
			// Don't add light color if there is an object blocking it's view of the light. (Shadows)
			Ray shadowRay(intersection.hitPos + 0.001f * direction, direction); // Account for shadow acne.
			Intersection shadowIntersection = FindIntersection(scene, shadowRay);
			// Need two checks in case the light is before the intersection.
			if (shadowIntersection.hitPos == vec4(FLT_MAX) || length(point->GetPosition() - shadowRay.GetPosition()) < length(shadowIntersection.hitPos - shadowRay.GetPosition()))
			{
				vec4 half = normalize(direction + towardsRayOrigin); // The vector halfway between the ray origin and light.
				currentColor += ComputeColor(direction, intersection.normal, half, point->color, intersection.material.diffuse, intersection.material.specular, intersection.material.shininess, point->attenuation, point->intensity, length(toLight));
			}
		}
	}

	// Add color from specular reflections.
	if (intersection.material.specular != vec3(0.0))
	{
		vec3 hitDir = intersection.ray.GetDirection();
		vec4 direction = vec4(normalize(hitDir - 2 * dot(hitDir, intersection.normal) * intersection.normal), 0);
		Ray reflectionRay(intersection.hitPos + 0.01f * direction, direction);
		currentColor += intersection.material.specular * ColorFromReflections(scene, reflectionRay, recursiveCall);
	}

	return currentColor;
}

vec3 RayTracer::ColorFromReflections(Scene* scene, const Ray& reflectionRay, int recursiveCall)
{
	if (recursiveCall >= m_maxRayDepth) { return vec3(0.0); } // Add nothing if we hit reflection depth.
	Intersection intersection = FindIntersection(scene, reflectionRay);
	return FindColor(scene, intersection, ++recursiveCall);
}
