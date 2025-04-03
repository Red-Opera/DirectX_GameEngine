#include "stdafx.h"
#include "SponzaScene.h"

#include "Core/Draw/Model.h"

SponzaScene::SponzaScene(std::string sceneName) : Scene(sceneName)
{

}

void SponzaScene::Initialize()
{
	std::shared_ptr<Object> gobber = AddObject(Object::Create("Gobber"));
	gobber->AddComponent<Model>("Model/Sample/gobber/GoblinX.obj", 4.0f);
	gobber->GetComponent<TransformComponent>()->SetPosition(-30.0f, 10.0f, 0.0f);
	gobber->GetComponent<TransformComponent>()->SetRotation(0.0f, -Math::PI / 2.0f, 0.0f);

	std::shared_ptr<Object> nano = AddObject(Object::Create("Nano"));
	nano->AddComponent<Model>("Model/Sample/nano_textured/nanosuit.obj", 1.0f);
	nano->AddComponent<TransformComponent>()->SetPosition(27.0f, -0.56f, 1.7f);
	nano->AddComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);

	std::shared_ptr<Object> sponza = AddObject(Object::Create("Sponza"));
	sponza->AddComponent<Model>("Model/Sample/sponza/sponza.obj", 1.0f / 20.0f);

	std::shared_ptr<Object> chair = AddObject(Object::Create("RoyalChair"));
	chair->AddComponent<Model>("Model/Sample/Chair/Chair.gltf", 7.0f);
	chair->GetComponent<TransformComponent>()->SetRotation(Math::PI / 2.0f, -Math::PI / 2.0f, 0.0f);
	chair->GetComponent<TransformComponent>()->SetPosition(-60.0f, 6.5f, 1.5f);

	Scene::Initialize();
}

std::shared_ptr<Scene> SponzaScene::Create(std::string sceneName)
{
	activeScene = std::make_shared<SponzaScene>(sceneName);

	return activeScene;
}
