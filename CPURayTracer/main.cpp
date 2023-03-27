#include "header/Renderer.h"
#include "header/SceneBuilder.h"

int main(int argc, char* argv[])
{
	/*if (argc < 2) {
		cout << "No Scene File Specified." << endl;
		exit(0);
	}*/

	const char* sceneFile = "testscenes/scene1.scene"; // "testscenes/scene4-specular.test" "testscenes/scene4.test"

	CustomSceneBuilder sceneBuilder;
	Scene* scene = sceneBuilder.buildFromFile(sceneFile);
	
	Renderer renderer(scene, sceneBuilder.GetOptions());
	renderer.RenderScene();
	renderer.SaveImage();
}