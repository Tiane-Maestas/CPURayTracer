#pragma once
#include "RenderingPipelineConfig.h"
#include <FreeImage.h>

class Image 
{
public:
	Image(FREE_IMAGE_FORMAT fif, std::string filepath);
	Image(uint32_t width, uint32_t height);
	void Save(std::string filename);
	// Getters
    vec3& operator()(int x) { return m_pixels[x]; }
    const vec3& operator()(int x) const { return m_pixels[x]; }
	vec3& operator()(int x, int y) { 
		if (y * m_width + x > m_width * m_height)
			return m_pixels[0];
		return m_pixels[y * m_width + x]; 
	}
    const vec3& operator()(int x, int y) const { return m_pixels[y * m_width + x]; }
	uint32_t GetWidth() { return m_width; }
	uint32_t GetHeight() { return m_height; }
private:
	// Pixel Buffer (origin in bottom left)
	std::vector<vec3> m_pixels;
	// Attributes
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	FREE_IMAGE_FORMAT m_fif = FIF_PNG; // Only currently supports png and jpeg.
};