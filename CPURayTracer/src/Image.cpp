#include "../header/Image.h";

vec3 TransformColorFromByte(BYTE* pixels, int pixel)
{
	return vec3(pixels[pixel + 2] / 255.0, pixels[pixel + 1] / 255.0, pixels[pixel] / 255.0); // B,G,R
}

Image::Image(FREE_IMAGE_FORMAT fif, std::string filepath)
{
	m_fif = fif;
	FIBITMAP* bitmap = FreeImage_Load(fif, filepath.c_str());
	BYTE* imagePixels = FreeImage_GetBits(bitmap);
	m_width = FreeImage_GetWidth(bitmap);
	m_height = FreeImage_GetHeight(bitmap);

	// Populate m_pixels.
	for (int pixel = 0; pixel < 3 * m_width * m_height; pixel += 3)
		m_pixels.push_back(TransformColorFromByte(imagePixels, pixel));
}

Image::Image(uint32_t width, uint32_t height)
{
	m_width = width; m_height = height;
	for (int i = 0; i < width * height; i++)
		m_pixels.push_back(vec3(0.0));
}

void TransformColorToByte(uint8_t* properColor, vec3 color)
{
	float r = color.x * 255;
	float g = color.y * 255;
	float b = color.z * 255;
	properColor[0] = (r > 255) ? 255 : (uint8_t)r;
	properColor[1] = (g > 255) ? 255 : (uint8_t)g;
	properColor[2] = (b > 255) ? 255 : (uint8_t)b;
}

void ColorPixel(BYTE* imagePixels, int pixel, const uint8_t* channels)
{
	imagePixels[pixel + 0] = channels[2]; // Blue
	imagePixels[pixel + 1] = channels[1]; // Green
	imagePixels[pixel + 2] = channels[0]; // Red
}

void Image::Save(std::string filename)
{
	// Write to BYTE array to save image.
	uint32_t totNumBytes = 3 * (m_width * m_height);
	BYTE* imagePixels = new uint8_t[totNumBytes];
	int pixel = 0;
	for (int i = 0; i < m_width * m_height; i++)
	{
		uint8_t color[3] = { 0, 0, 0 };
		TransformColorToByte(color, m_pixels[i]);
		ColorPixel(imagePixels, pixel, color);
		pixel += 3;
	}

	FIBITMAP* img = FreeImage_ConvertFromRawBits(imagePixels, m_width, m_height, 3 * m_width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
}
