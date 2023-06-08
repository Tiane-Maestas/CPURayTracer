#pragma once
#include "RenderingPipelineConfig.h"
#include <FreeImage.h>

class Image 
{
public:
	Image() { m_hasImage = false; } // If created with default constructor there is no image.
	Image(FREE_IMAGE_FORMAT fif, std::string filepath);
	Image(uint32_t width, uint32_t height);
	void Save(std::string filename);
	// Getters
    vec3& operator()(int x) { return m_pixels[x]; }
    const vec3& operator()(int x) const { return m_pixels[x]; }
	vec3& operator()(int x, int y) { return (y * m_width + x > m_pixels.size()) ? m_pixels[m_pixels.size() - 1] : m_pixels[y * m_width + x]; }
    const vec3& operator()(int x, int y) const { return (y * m_width + x > m_pixels.size()) ? m_pixels[m_pixels.size() - 1] : m_pixels[y * m_width + x]; }
	const vec3& QueryTexture(float u, float v) const;
	const uint32_t GetWidth() const { return m_width; }
	const uint32_t GetHeight() const { return m_height; }
	const bool HasImage() const { return m_hasImage; }
private:
	// Pixel Buffer (origin in bottom left)
	std::vector<vec3> m_pixels;
	// Attributes
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	FREE_IMAGE_FORMAT m_fif = FIF_PNG;
	bool m_hasImage = true;
	// Texture attributes.
	float m_uscale = 1.0f;
	float m_vscale = 1.0f;
	float m_uoffset = 0.0f;
	float m_voffset = 0.0f;
};
