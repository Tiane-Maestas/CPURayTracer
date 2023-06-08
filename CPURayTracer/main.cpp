#include "header/Renderer.h"
#include "header/SceneBuilder.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "No Scene File Specified." << std::endl;
		exit(0);
	}

	const char* sceneFile = argv[1];

	MyCustomSceneBuilder sceneBuilder;
	std::shared_ptr<Scene> scene = sceneBuilder.BuildFromFile(sceneFile);
	if (scene == nullptr) { exit(1); }
	
	Renderer renderer(scene, sceneBuilder.GetOptions());
	renderer.RenderScene();
	renderer.SaveImage();

	RenderingStatistics::Print();
}