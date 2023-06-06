#include "../header/Skybox.h"
#include <filesystem>

using namespace EnviornmentMap;

Skybox::Skybox(FREE_IMAGE_FORMAT fif, std::string filepath)
{
	for (const auto& entry : std::filesystem::directory_iterator(filepath))
		m_enviornmentTextures.push_back(Image(fif, entry.path().string()));

	m_type = (m_enviornmentTextures.size() > 1) ? SkyboxType::Cube : SkyboxType::Sphere;
}

vec3 Skybox::Query(vec3 direction)
{
	if (m_type == SkyboxType::Cube) 
	{
		float axis = std::max(fabs(direction.x), std::max(fabs(direction.y), fabs(direction.z))); // Find the major axis 'direction' is pointing.
		if (axis == fabs(direction.x))
		{
			if (direction.x > 0)
			{
				Image& boxSide = m_enviornmentTextures[2]; // Positive X
				float u = 1.0f - (direction.z / direction.x + 1.0f) * 0.5f;
				float v = (direction.y / direction.x + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else 
			{
				Image& boxSide = m_enviornmentTextures[3]; // Negative X
				float u = 1.0f - (direction.z / direction.x + 1.0f) * 0.5f;
				float v = 1.0f - (direction.y / direction.x + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
		else if (axis == fabs(direction.y))
		{
			if (direction.y > 0)
			{
				Image& boxSide = m_enviornmentTextures[4]; // Positive Y
				float u = (direction.x / direction.y + 1.0f) * 0.5f;
				float v = 1.0f - (direction.z / direction.y + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else
			{
				Image& boxSide = m_enviornmentTextures[5]; // Negative Y
				float u = 1.0f - (direction.x / direction.y + 1.0f) * 0.5f;
				float v = 1.0f - (direction.z / direction.y + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
		else
		{
			if (direction.z > 0)
			{
				Image& boxSide = m_enviornmentTextures[0]; // Positive Z
				float u = (direction.x / direction.z + 1.0f) * 0.5f;
				float v = (direction.y / direction.z + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else
			{
				Image& boxSide = m_enviornmentTextures[1]; // Negative Z
				float u = (direction.x / direction.z + 1.0f) * 0.5f;
				float v = 1.0f - (direction.y / direction.z + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
	}
	
	// Sphere skymap.
	float u = 0.5f + atan2(direction.z, direction.x) / (2 * pi);
	float v = 0.5f + asin(direction.y) / pi;
	return m_enviornmentTextures[0](m_enviornmentTextures[0].GetWidth() * u, m_enviornmentTextures[0].GetHeight() * v);
}
