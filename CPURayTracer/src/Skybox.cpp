#include "../header/Skybox.h"
#include <filesystem>

using namespace EnviornmentMap;

Skybox::Skybox(FREE_IMAGE_FORMAT fif, std::string filepath, SkyboxType type)
{
	m_type = type;
	for (const auto& entry : std::filesystem::directory_iterator(filepath))
		m_enviornmentTextures.push_back(Image(fif, entry.path().string()));
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
				Image& boxSide = m_enviornmentTextures[0]; // Positive X
				float u = 1.0f - (direction.z / direction.x + 1.0f) * 0.5f;
				float v = (direction.y / direction.x + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else 
			{
				Image& boxSide = m_enviornmentTextures[1]; // Negative X
				float u = 1.0f - (direction.z / direction.x + 1.0f) * 0.5f;
				float v = 1.0f - (direction.y / direction.x + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
		else if (axis == fabs(direction.y))
		{
			if (direction.y > 0)
			{
				Image& boxSide = m_enviornmentTextures[2]; // Positive Y
				float u = 1.0f - (direction.z / direction.y + 1.0f) * 0.5f;
				float v = 1.0f - (direction.x / direction.y + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else
			{
				Image& boxSide = m_enviornmentTextures[3]; // Negative Y
				float u = (direction.z / direction.y + 1.0f) * 0.5f;
				float v = 1.0f - (direction.x / direction.y + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
		else
		{
			if (direction.z > 0)
			{
				Image& boxSide = m_enviornmentTextures[4]; // Positive Z
				float u = (direction.x / direction.z + 1.0f) * 0.5f;
				float v = (direction.y / direction.z + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
			else
			{
				Image& boxSide = m_enviornmentTextures[5]; // Negative Z
				float u = (direction.x / direction.z + 1.0f) * 0.5f;
				float v = 1.0f - (direction.y / direction.z + 1.0f) * 0.5f;
				return boxSide(boxSide.GetWidth() * u, boxSide.GetHeight() * v);
			}
		}
	}
	
	return vec3(0.0);
}