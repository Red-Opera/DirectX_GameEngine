#include "stdafx.h"
#include "EmptyScene.h"

EmptyScene::EmptyScene(std::string sceneName) : Scene(sceneName)
{

}

std::shared_ptr<Scene> EmptyScene::Create(std::string sceneName)
{
	activeScene = std::make_shared<EmptyScene>(sceneName);

	return activeScene;
}
