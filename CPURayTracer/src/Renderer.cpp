#include "../header/Renderer.h";

Renderer::Renderer(Scene* scene, RenderingOptions options)
{
	m_scene = scene;
	m_options = options;
	m_totNumBytes = 3 * (options.ImageWidth * options.ImageHeight);
	m_imagePixels = new uint8_t[m_totNumBytes];

	// Set number of threads to use.
	m_numThreads = thread::hardware_concurrency();
	if (options.NumberOfUsedThreads == Amount::None)
	{
		m_numThreads = 0;
	}
	else if (options.NumberOfUsedThreads == Amount::Single)
	{
		m_numThreads = 1;
	}
	else if (options.NumberOfUsedThreads == Amount::Half)
	{
		m_numThreads = m_numThreads / 2;
	}

	if(m_numThreads != 0)
		m_blockSize = min(options.ImageWidth, options.ImageHeight) / (4 * m_numThreads); // Take the min in case of weird aspect ratios.
}

Renderer::~Renderer()
{
	//delete m_imagePixels;
}

void Renderer::RenderScene()
{
	// First update the camera fov in the x-direction by the world aspect ratio.
	m_scene->camera.setFOVx(m_options.ImageWidth, m_options.ImageHeight);

	// Transform object if needed.
	m_scene->TransformObjects();

	Timer traceImage("TraceImage");

	if (m_numThreads == 0)
	{
		RayTracer rayTracer(m_options.ImageWidth, m_options.ImageHeight);
		PixelBlock block{ 0, 0, m_options.ImageWidth, m_options.ImageHeight };
		rayTracer.TraceImage(m_scene, block);
		m_imageBlocks.push_back(rayTracer.getImage());
		m_imagePixels = m_imageBlocks[0].get();
		return;
	}

	// Create a RayTracer for each thread.
	vector<RayTracer> rayTracers;
	for (int i = 0; i < m_numThreads; i++)
	{
		rayTracers.push_back(RayTracer(m_options.ImageWidth, m_options.ImageHeight));
	}

	// Reserve a list of blocks for each thread.
	vector<vector<PixelBlock>> blocksList(m_numThreads, vector<PixelBlock>());

	// Populate the list of blocks.
	int currThread = 0;
	int xlimit, ylimit;
	for (int y = 0; y < m_options.ImageHeight; y += m_blockSize)
	{
		// Correct for block size overflow.
		ylimit = y + m_blockSize;
		if (ylimit > m_options.ImageHeight) { ylimit = m_options.ImageHeight; }

		for (int x = 0; x < m_options.ImageWidth; x += m_blockSize)
		{
			// Correct for block size overflow.
			xlimit = x + m_blockSize;
			if (xlimit > m_options.ImageWidth) { xlimit = m_options.ImageWidth; }

			PixelBlock currentBlock{ x, y, xlimit, ylimit };
			blocksList[currThread].push_back(currentBlock);
			currThread = (currThread < m_numThreads - 1) ? currThread + 1 : 0;
		}
	}

	// Trace each list of blocks.
	for (int i = 0; i < m_numThreads; i++)
	{
		rayTracers[i].TraceImage(m_scene, blocksList[i]);
	}

	// Wait for each tracer to be done.
	for (int i = 0; i < m_numThreads; i++)
	{
		while (rayTracers[i].isTracing())
		{
			// Do Nothing
		}
		m_imageBlocks.push_back(rayTracers[i].getImage());
	}
	
	ConstructPixelsFromBlocks();
}

void Renderer::ConstructPixelsFromBlocks() 
{
	m_imagePixels = m_imageBlocks[0].get();
	for (int i = 1; i < m_imageBlocks.size(); i++)
	{
		for (int j = 0; j < m_totNumBytes; j++)
		{
			m_imagePixels[j] += m_imageBlocks[i].get()[j];
		}
	}
}

void Renderer::SaveImage()
{
	FIBITMAP* img = FreeImage_ConvertFromRawBits(m_imagePixels, m_options.ImageWidth, m_options.ImageHeight, 3 * m_options.ImageWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false); // If false then bottom left orign.
	FreeImage_Save(FIF_PNG, img, m_options.OutputFileName.c_str(), 0);
	cout << "Image Saved: " << m_options.OutputFileName << endl;
}
