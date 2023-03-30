#include "../header/Renderer.h";

Renderer::Renderer(Scene* scene, RenderingOptions options)
{
	m_scene = scene;
	m_options = options;
	m_numBytes = 3 * (options.ImageWidth * options.ImageHeight);
	m_imagePixels = new BYTE[m_numBytes];
}

Renderer::~Renderer()
{
	delete m_imagePixels;
}

void Renderer::RenderScene()
{
	// First update the camera fov in the x-direction by the world aspect ratio.
	m_scene->camera.setFOVx(m_options.ImageWidth, m_options.ImageHeight);

	// Transform object if needed.
	m_scene->TransformObjects();

	RayTracer rayTracer(m_options.ImageWidth, m_options.ImageHeight);
	Timer traceImage("TraceImage");
	rayTracer.TraceImage(m_scene);
	memcpy(m_imagePixels, rayTracer.getImage(), m_numBytes);
}

void Renderer::SaveImage()
{
	FIBITMAP* img = FreeImage_ConvertFromRawBits(m_imagePixels, m_options.ImageWidth, m_options.ImageHeight, 3 * m_options.ImageWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, m_options.OutputFileName.c_str(), 0);
	cout << "Image Saved: " << m_options.OutputFileName << endl;
}
