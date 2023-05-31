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
		float axis = std::max(fabs(direction.x), std::max(fabs(direction.y), fabs(direction.z))); // Find the axis direction is pointing.
		if (axis == fabs(direction.x)) // X
		{
			direction = direction / direction.x;
			if (direction.x > 0) // Positive
			{
				Image& boxSide = m_enviornmentTextures[0];
				return boxSide(boxSide.GetWidth() * direction.z, boxSide.GetHeight() * direction.y);
			}
			else // Negative 
			{
				Image& boxSide = m_enviornmentTextures[1];
				return boxSide(boxSide.GetWidth() * -1.0 * direction.z, boxSide.GetHeight() * -1.0 * direction.y);
			}
		}
		else if (axis == fabs(direction.y)) // Y
		{
			direction = direction / direction.y;
			if (direction.y > 0) // Positive
			{
				Image& boxSide = m_enviornmentTextures[2];
				return boxSide(boxSide.GetWidth() * direction.x, boxSide.GetHeight() * direction.z);
			}
			else // Negative 
			{
				Image& boxSide = m_enviornmentTextures[3];
				return boxSide(boxSide.GetWidth() * -1.0 * direction.x, boxSide.GetHeight() * -1.0 * direction.z);
			}
		}
		else // Z
		{
			direction = direction / direction.z;
			if (direction.z > 0) // Positive
			{
				Image& boxSide = m_enviornmentTextures[4];
				return boxSide(boxSide.GetWidth() * direction.x, boxSide.GetHeight() * direction.y);
			}
			else // Negative 
			{
				Image& boxSide = m_enviornmentTextures[5];
				return boxSide(boxSide.GetWidth() * -1.0 * direction.x, boxSide.GetHeight() * -1.0 * direction.y);
			}
		}
	}
	
	return vec3(0.0);
}
