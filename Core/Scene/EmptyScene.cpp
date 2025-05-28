#include "stdafx.h"
#include "EmptyScene.h"

#include "Core/Object/Object.h"
#include "Core/Component/Component.h"
#include "Core/Camera/Camera.h"

EmptyScene::EmptyScene(std::string sceneName) : Scene(sceneName)
{

}

std::shared_ptr<Scene> EmptyScene::Create(std::string sceneName)
{
	activeScene = std::make_shared<EmptyScene>(sceneName);

	return activeScene;
}

void EmptyScene::Initialize()
{
	std::shared_ptr<Object> camera = AddObject(Object::Create("Empty Scene Camera"));
	cameras.AddCamera(camera->AddComponent<Camera>());
	camera->GetComponent<TransformComponent>()->SetPosition(-22.0f, 4.0f, 0.0f);
	camera->GetComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);

	Scene::Initialize();
}