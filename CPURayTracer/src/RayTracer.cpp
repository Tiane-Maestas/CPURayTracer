#include "../header/RayTracer.h"

RayTracer::RayTracer(uint8_t* imagePixels, float width, float height)
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

			uint8_t color[3] = { 0, 0, 0 };
			vec3 col = FindColor(scene, intersection, 0);
			TransformColor(color, col);

			ColorPixel(pixel, color);
			pixel += 3;
		}
		pixel += 3 * (m_imageWidth - (block.xMax - block.xMin));
	}
}

void RayTracer::TraceImage(Scene* scene, std::vector<PixelBlock> blocks)
{
	m_thread = std::make_shared<std::thread>(&RayTracer::TraceThreadedImage, this, scene, blocks);
}

void RayTracer::TraceThreadedImage(Scene* scene, std::vector<PixelBlock> blocks)
{
	for (PixelBlock block : blocks) 
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
	float alpha = tan(scene->camera.getFOV().x / 2) * (a / (m_imageWidth / 2));
	float beta = tan(scene->camera.getFOV().y / 2) * (b / (m_imageHeight / 2));
	vec3 w = normalize(scene->camera.getPosition() - scene->camera.getLookingAt());
	vec3 u = normalize(cross(vec3(scene->camera.getUp()), w));
	vec3 v = normalize(cross(w, u));

	vec3 dir = alpha * u + beta * v - w;
	dir = normalize(dir);

	return Ray(scene->camera.getPosition(), dir);
}

// ---Start of Intersection Section---
void MeshIntersectionTest(std::shared_ptr<Mesh> mesh, Ray& ray, vec4& closestHit, int& closestTriangleId, std::shared_ptr<Mesh>& closestMesh)
{
	vec3 rayOrigin = ray.getPosition();
	vec3 rayDir = ray.getDirection();
	int index = 0;
	// Check if intersect any of its triangles.
	for (Triangle tri : mesh->triangles)
	{
		if (RenderingStatistics::SaveStatistics)
			RenderingStatistics::NumTriTests++;

		vec3 normal = tri.getFlatNormal();
		float denominator = dot(rayDir, normal);
		if (denominator != 0) // The ray hits the plane of the triangle if this isn't zero.
		{ 
			vec3 a = tri.A(); vec3 b = tri.B(); vec3 c = tri.C();
			float t = (dot(a, normal) - dot(rayOrigin, normal)) / denominator;
			if (t > 0) // The triangle is in front of ray and not behind.
			{ 
				vec3 pos = ray.At(t);

				vec3 _a = cross(normal, c - b);
				vec3 A = _a / dot(_a, a - c);

				vec3 _b = cross(normal, a - c);
				vec3 B = _b / dot(_b, b - a);

				vec3 _c = cross(normal, b - a);
				vec3 C = _c / dot(_c, c - b);

				float alpha = dot(A, pos) - dot(A, c);
				float beta = dot(B, pos) - dot(B, a);
				float gamma = dot(C, pos) - dot(C, b);
				if (alpha >= 0 && beta >= 0 && gamma >= 0) // If true. Inside triangle.
				{
					if (length(vec4(pos, 1) - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check. Note: length uses dot product so dimensionality matters.
					{
						closestHit = vec4(pos, 1);
						closestMesh = mesh;
						closestTriangleId = index;
					}
				}
			}
		}
		index++;
	}
}

void SphereIntersectionTest(Sphere* sph, std::shared_ptr<Mesh> mesh, Ray& ray, vec4& closestHit, int& closestTriangleId, std::shared_ptr<Mesh>& closestMesh)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumSphTests++;

	vec3 rayOrigin = ray.getPosition();
	vec3 rayDir = ray.getDirection();

	vec3 sphPos = sph->getPosition();

	float a = dot(rayDir, rayDir);
	float b = 2 * dot(rayDir, rayOrigin - sphPos);
	float c = dot(rayOrigin - sphPos, rayOrigin - sphPos) - pow(sph->getRadius(), 2);
	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0) // Hit Sphere
	{
		if (discriminant == 0) // Tangent to sphere.
		{
			float t = -0.5 * b / a;

			vec4 pos = ray.At(t);
			if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
			{
				closestHit = pos;
				closestMesh = mesh;
				closestTriangleId = 0;
			}
		}
		else 
		{
			float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
			float t1 = q / a;
			float t2 = c / q;
			if (t1 > 0 && t2 > 0) // Hit sphere in two places.
			{
				t1 = std::min(t1, t2);

				vec4 pos = ray.At(t1);
				if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
				{
					closestHit = pos;
					closestMesh = mesh;
					closestTriangleId = 0;
				}
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0) 
				{
					vec4 pos = ray.At(t2);
					if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
					{
						closestHit = pos;
						closestMesh = mesh;
						closestTriangleId = 0;
					}
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0) 
				{
					vec4 pos = ray.At(t1);
					if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
					{
						closestHit = pos;
						closestMesh = mesh;
						closestTriangleId = 0;
					}
				}
			} // Two negative roots the sphere is behind.
		}
	}
}

void EllipsoidIntersectionTest(Ellipsoid* elp, std::shared_ptr<Mesh> mesh, Ray& ray, vec4& closestHit, int& closestTriangleId, std::shared_ptr<Mesh>& closestMesh)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::NumElpTests++;

	// Transform ray since ellipsoid's aren't transformed in scene.
	ray.Transform(inverse(elp->getTransform()));

	vec3 rayOrigin = ray.getPosition();
	vec3 rayDir = ray.getDirection();

	vec3 sphPos = elp->getPosition();

	float a = dot(rayDir, rayDir);
	float b = 2 * dot(rayDir, rayOrigin - sphPos);
	float c = dot(rayOrigin - sphPos, rayOrigin - sphPos) - pow(elp->getRadius(), 2);
	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0) // Hit Sphere
	{
		if (discriminant == 0) // Tangent to sphere.
		{
			float t = -0.5 * b / a;

			// Transform ray hit back to world coordintes.
			vec4 pos = ray.At(t);
			pos = elp->getTransform() * pos;
			if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
			{
				closestHit = pos;
				closestMesh = mesh;
				closestTriangleId = 0;
			}
		}
		else
		{
			float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
			float t1 = q / a;
			float t2 = c / q;
			if (t1 > 0 && t2 > 0) // Hit sphere in two places.
			{
				t1 = std::min(t1, t2);

				// Transform ray hit back to world coordintes.
				vec4 pos = ray.At(t1);
				pos = elp->getTransform() * pos;
				if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
				{
					closestHit = pos;
					closestMesh = mesh;
					closestTriangleId = 0;
				}
			}
			else if (t1 < 0) // Inside Sphere case 1.
			{
				if (t2 > 0)
				{
					// Transform ray hit back to world coordintes.
					vec4 pos = ray.At(t2);
					pos = elp->getTransform() * pos;
					if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
					{
						closestHit = pos;
						closestMesh = mesh;
						closestTriangleId = 0;
					}
				}
			}
			else if (t2 < 0) // Inside Sphere case 2.
			{
				if (t1 > 0)
				{
					// Transform ray hit back to world coordintes.
					vec4 pos = ray.At(t1);
					pos = elp->getTransform() * pos;
					if (length(pos - ray.getPosition()) < length(closestHit - ray.getPosition())) // Depth Check.
					{
						closestHit = pos;
						closestMesh = mesh;
						closestTriangleId = 0;
					}
				}
			} // Two negative roots the sphere is behind.
		}
	}

	// Transform Ray Back so it is in world coordinates for next pass.
	ray.Transform(elp->getTransform());
}

Intersection RayTracer::FindIntersection(Scene* scene, Ray& ray)
{
	if (RenderingStatistics::SaveStatistics)
		RenderingStatistics::RaysCasted++;

	// These are for distance checks.
	vec4 closestHit(FLT_MAX);
	std::shared_ptr<Mesh> closestMesh = nullptr;
	int closestTriangleId = 0;

	// For each mesh in the scene see if the ray intersects it.
	for (std::shared_ptr<Mesh> mesh : scene->meshes)
	{
		// Note: Make sure these checks follow inheritance.
		if (Ellipsoid* elp = dynamic_cast<Ellipsoid*>(mesh.get()))
		{
			EllipsoidIntersectionTest(elp, mesh, ray, closestHit, closestTriangleId, closestMesh);
		}
		else if (Sphere* sph = dynamic_cast<Sphere*>(mesh.get())) 
		{
			SphereIntersectionTest(sph, mesh, ray, closestHit, closestTriangleId, closestMesh);
		}
		else // If it isn't any primitive mesh then it's a plain mesh.
		{
			MeshIntersectionTest(mesh, ray, closestHit, closestTriangleId, closestMesh);
		}
	}

	return Intersection{ ray, closestHit, closestTriangleId, closestMesh };
}
// ---End of Intersection Section---

// Helper function for 'FindColor'
vec3 ComputeColor(const vec3 direction, const vec3 normal, const vec3 halfvec, const vec3 color, const vec3 diffuse, const vec3 specular, const float shininess, const vec3 attenuation, const float intensity, const float distance)
{
	float nDotL = dot(normal, direction);
	vec3 lambert = diffuse * color * std::max(nDotL, 0.0f);

	float nDotH = dot(normal, halfvec);
	vec3 phong = specular * color * pow(std::max(nDotH, 0.0f), shininess);

	float attenuate = intensity / (attenuation.x + attenuation.y * distance + attenuation.z * pow(distance, 2));

	return attenuate * (lambert + phong);
}

vec3 RayTracer::FindColor(Scene* scene, Intersection intersection, int recursiveCall)
{
	if (intersection.hitMesh == nullptr) { return m_defaultColor; } // Return background if hit nothing.

	Material hitMaterial = intersection.hitMesh.get()->material;
	vec4 pos = intersection.hitPos;
	vec3 normal = intersection.hitMesh.get()->getNormal(intersection.triangleId, pos);

	// Perfect Mirror reflections.
	if (hitMaterial.type == MaterialType::Mirror)
	{
		vec3 hitDir = intersection.ray.getDirection();
		vec4 direction = vec4(normalize(hitDir - 2 * dot(hitDir, normal) * normal), 0);
		Ray reflectionRay(pos + 0.01f * direction, direction);
		return hitMaterial.ambient * ColorFromReflections(scene, reflectionRay, recursiveCall);
	}

	vec3 currentColor = hitMaterial.ambient + hitMaterial.emission;

	// Add color from the lights.
	vec4 towardsRayOrigin = normalize(intersection.ray.getPosition() - pos); // Vector to origin of ray cast. Needed for half vector.
	for (std::shared_ptr<Light> light : scene->lights)
	{
		if (Directional* directional = dynamic_cast<Directional*>(light.get()))
		{
			vec4 direction = normalize(directional->getPosition());
			
			// Don't add light color if there is an object blocking it's view of the light. (Shadows)
			Ray shadowRay(pos + 0.001f * direction, direction); // Account for shadow acne.
			Intersection shadowIntersection = FindIntersection(scene, shadowRay);
			if (shadowIntersection.hitMesh == nullptr) // Direction lights only have one check becuase they have no position.
			{
				vec4 half = normalize(direction + towardsRayOrigin); // The vector halfway between the ray origin and light.
				currentColor += ComputeColor(direction, normal, half, directional->color, hitMaterial.diffuse, hitMaterial.specular, hitMaterial.shininess, Directional::attenuation, 1.0f, 0.0f);
			}
		}
		else if (Point* point = dynamic_cast<Point*>(light.get()))
		{
			vec4 toLight = point->getPosition() - pos;
			vec4 direction = normalize(toLight); // Direction to light.
			
			// Don't add light color if there is an object blocking it's view of the light. (Shadows)
			Ray shadowRay(pos + 0.001f * direction, direction); // Account for shadow acne.
			Intersection shadowIntersection = FindIntersection(scene, shadowRay);
			// Need two checks in case the light is before the intersection.
			if (shadowIntersection.hitMesh == nullptr || length(point->getPosition() - shadowRay.getPosition()) < length(shadowIntersection.hitPos - shadowRay.getPosition())) 
			{
				vec4 half = normalize(direction + towardsRayOrigin); // The vector halfway between the ray origin and light.
				currentColor += ComputeColor(direction, normal, half, point->color, hitMaterial.diffuse, hitMaterial.specular, hitMaterial.shininess, point->attenuation, point->intensity, length(toLight));
			}
		}
	}

	// Add color from reflections.
	if (hitMaterial.specular != vec3(0.0))
	{
		vec3 hitDir = intersection.ray.getDirection();
		vec4 direction = vec4(normalize(hitDir - 2 * dot(hitDir, normal) * normal), 0);
		Ray reflectionRay(pos + 0.01f * direction, direction);
		currentColor += hitMaterial.specular * ColorFromReflections(scene, reflectionRay, recursiveCall);
	}

	return currentColor;
}

vec3 RayTracer::ColorFromReflections(Scene* scene, Ray reflectionRay, int recursiveCall)
{
	if (recursiveCall >= m_maxRayDepth) { return vec3(0.0); } // Add nothing if we hit reflection depth.
	Intersection intersection = FindIntersection(scene, reflectionRay);
	return FindColor(scene, intersection, ++recursiveCall);
}

void RayTracer::TransformColor(uint8_t* properColor, vec3 color)
{
	float r = color.x * 255;
	float g = color.y * 255;
	float b = color.z * 255;
	properColor[0] = (r > 255) ? 255 : (uint8_t)r;
	properColor[1] = (g > 255) ? 255 : (uint8_t)g;
	properColor[2] = (b > 255) ? 255 : (uint8_t)b;
}

void RayTracer::ColorPixel(int pixel, const uint8_t* channels)
{
	m_imagePixels[pixel + 0] = channels[2]; // Blue
	m_imagePixels[pixel + 1] = channels[1]; // Green
	m_imagePixels[pixel + 2] = channels[0]; // Red
}
