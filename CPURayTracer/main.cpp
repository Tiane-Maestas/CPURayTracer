#include "header/Renderer.h"
#include "header/SceneBuilder.h"

int main(int argc, char* argv[])
{
	/*if (argc < 2) {
		cout << "No Scene File Specified." << endl;
		exit(0);
	}*/

	const char* sceneFile = "testscenes/scene2.scene";

	CustomSceneBuilder sceneBuilder;
	Scene* scene = sceneBuilder.BuildFromFile(sceneFile);
	if (scene == nullptr) { exit(1); }
	
	Renderer renderer(scene, sceneBuilder.GetOptions());
	renderer.RenderScene();
	renderer.SaveImage();

	RenderingStatistics::Print();
}