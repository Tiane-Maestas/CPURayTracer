#pragma once
#include "RayTracerCore.h"
#include "Image.h"

namespace EnviornmentMap
{
	enum SkyboxType
	{
		Cube, // 6 enviornment textures (+x, -x, +y, -y, +z, -z)
		Sphere // 1 enviornment texture
	};

	class Skybox
	{
	public:
		Skybox() {}
		Skybox(FREE_IMAGE_FORMAT fif, std::string filepath, SkyboxType type);
		vec3 Query(vec3 direction);
	private:
		SkyboxType m_type = SkyboxType::Cube;
		std::vector<Image> m_enviornmentTextures;
	};

}