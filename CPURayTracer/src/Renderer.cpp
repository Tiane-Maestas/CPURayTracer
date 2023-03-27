#include "../header/Renderer.h";

Renderer::Renderer(Scene* scene, RenderingOptions options)
{
	this->scene = scene;
	this->options = options;
	this->imagePixels = new BYTE[3 * (options.ImageWidth * options.ImageHeight)];
}

Renderer::~Renderer()
{
	delete scene;
	delete imagePixels;
}

void Renderer::RenderScene()
{
}

void Renderer::SaveImage()
{
	FIBITMAP* img = FreeImage_ConvertFromRawBits(imagePixels, options.ImageWidth, options.ImageHeight, 3 * options.ImageWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, options.OutputFileName.c_str(), 0);
	cout << "Image Saved: " << options.OutputFileName << endl;
}
