#include "../header/Renderer.h";

Renderer::Renderer(std::shared_ptr<Scene> scene, RenderingOptions options)
{
	m_scene = scene;
	m_options = options;
	m_totNumBytes = 3 * (options.ImageWidth * options.ImageHeight);
	m_imagePixels = new uint8_t[m_totNumBytes];

	// Set number of threads to use.
	m_numThreads = std::thread::hardware_concurrency();
	if (options.NumberOfUsedThreads == ThreadAmount::None)
	{
		m_numThreads = 0;
	}
	else if (options.NumberOfUsedThreads == ThreadAmount::Single)
	{
		m_numThreads = 1;
	}
	else if (options.NumberOfUsedThreads == ThreadAmount::Half)
	{
		m_numThreads = m_numThreads / 2;
	}

	if(m_numThreads != 0)
		m_blockSize = std::min(options.ImageWidth, options.ImageHeight) / (4 * m_numThreads); // Take the min in case of weird aspect ratios.
}

Renderer::~Renderer() 
{
	delete m_imagePixels;
}

void Renderer::RenderScene()
{
	// First update the camera fov in the x-direction by the world aspect ratio.
	m_scene->camera.SetFOV((float)m_options.ImageWidth, (float)m_options.ImageHeight);

	// Transform object if needed.
	m_scene->TransformObjects();

	// Prepare Acceleration.
	Timer bvhBuild("Build BVH");
	m_scene.get()->BuildAccelratedDatabase();
	bvhBuild.stop();

	Timer traceImage("TraceImage");

	if (m_numThreads == 0) // Run on the main thread.
	{
		RayTracer rayTracer(m_imagePixels, (float)m_options.ImageWidth, (float)m_options.ImageHeight);
		PixelBlock block{ 0, 0, m_options.ImageWidth, m_options.ImageHeight };
		rayTracer.TraceImage(m_scene.get(), block);
		return;
	}

	// Create a RayTracer for each thread.
	std::vector<RayTracer> rayTracers;
	for (uint32_t i = 0; i < m_numThreads; i++)
	{
		rayTracers.push_back(RayTracer(m_imagePixels, (float)m_options.ImageWidth, (float)m_options.ImageHeight));
	}

	// Reserve a list of blocks for each thread.
	std::vector<std::vector<PixelBlock>> blocksList(m_numThreads, std::vector<PixelBlock>());

	// Populate the list of blocks.
	uint32_t currThread = 0;
	uint32_t xlimit, ylimit;
	for (uint32_t y = 0; y < m_options.ImageHeight; y += m_blockSize)
	{
		// Correct for block size overflow.
		ylimit = y + m_blockSize;
		if (ylimit > m_options.ImageHeight) { ylimit = m_options.ImageHeight; }

		for (uint32_t x = 0; x < m_options.ImageWidth; x += m_blockSize)
		{
			// Correct for block size overflow.
			xlimit = x + m_blockSize;
			if (xlimit > m_options.ImageWidth) { xlimit = m_options.ImageWidth; }

			PixelBlock currentBlock{ x, y, xlimit, ylimit };
			blocksList[currThread].push_back(currentBlock);
			ProgressReport::TotalBlocks++;

			// Split amongst threads.
			currThread = (currThread < m_numThreads - 1) ? currThread + 1 : 0;
		}
	}

	// Trace each list of blocks on each RayTracer's thread.
	for (uint32_t i = 0; i < m_numThreads; i++)
	{
		rayTracers[i].TraceImage(m_scene.get(), blocksList[i]);
	}

	// Wait for each tracer to be done.
	for (uint32_t i = 0; i < m_numThreads; i++)
	{
		rayTracers[i].join();
	}
}

void Renderer::SaveImage()
{
	FIBITMAP* img = FreeImage_ConvertFromRawBits(m_imagePixels, m_options.ImageWidth, m_options.ImageHeight, 3 * m_options.ImageWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, m_options.OutputFileName.c_str(), 0);
	std::cout << "Image Saved: " << m_options.OutputFileName << std::endl;
}
