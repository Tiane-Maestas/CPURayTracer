#pragma once
#include "RayTracerCore.h"
#include "Image.h"

namespace EnviornmentMap
{
	enum SkyboxType
	{
		Cube, // 6 enviornment textures (+z, -z, +x, -x, +y, -y)
		Sphere // 1 enviornment texture
	};

	class Skybox
	{
	public:
		Skybox() {}
		Skybox(FREE_IMAGE_FORMAT fif, std::string filepath);
		vec3 Query(vec3 direction);
		float intensity = 1.0f; // Allows manual adjustments.
	private:
		SkyboxType m_type = SkyboxType::Cube;
		std::vector<Image> m_enviornmentTextures;
	};

}