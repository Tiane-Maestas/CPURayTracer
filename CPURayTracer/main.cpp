#include "header/Renderer.h"
#include "header/SceneBuilder.h"

int main(int argc, char* argv[])
{
	/*if (argc < 2) {
		cout << "No Scene File Specified." << endl;
		exit(0);
	}*/

	const char* sceneFile = "testscenes/Dragon.txt";

	CustomSceneBuilder sceneBuilder;
	Scene* scene = sceneBuilder.BuildFromFile(sceneFile);
	
	Renderer renderer(scene, sceneBuilder.GetOptions());
	renderer.RenderScene();
	renderer.SaveImage();
}