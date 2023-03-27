#include "../header/Renderer.h";

Renderer::Renderer(Scene* scene, RenderingOptions options)
{
	m_scene = scene;
	m_options = options;
	m_imagePixels = new BYTE[3 * (options.ImageWidth * options.ImageHeight)];
}

Renderer::~Renderer()
{
	delete m_scene;
	delete m_imagePixels;
}

void Renderer::RenderScene()
{
}

void Renderer::SaveImage()
{
	FIBITMAP* img = FreeImage_ConvertFromRawBits(m_imagePixels, m_options.ImageWidth, m_options.ImageHeight, 3 * m_options.ImageWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, m_options.OutputFileName.c_str(), 0);
	cout << "Image Saved: " << m_options.OutputFileName << endl;
}
