#include "stdafx.h"
#include "SponzaScene.h"

#include "Core/Camera/Camera.h"
#include "Core/Component/PhysicsComponent.h"
#include "Core/Draw/Light/PointLight.h"
#include "Core/Draw/Model.h"
#include "Core/Draw/Object/ColorCubeObject.h"
#include "Core/App.h"

SponzaScene::SponzaScene(std::string sceneName) : Scene(sceneName)
{

}

void SponzaScene::Initialize()
{
	std::shared_ptr<Object> camera = AddObject(Object::Create("Camera"));
	cameras.AddCamera(camera->AddComponent<Camera>());
	camera->GetComponent<TransformComponent>()->SetPosition(-22.0f, 4.0f, 0.0f);
	camera->GetComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);

	std::shared_ptr<Object> camera2 = AddObject(Object::Create("Camera2"));
	cameras.AddCamera(camera2->AddComponent<Camera>());
	camera2->GetComponent<TransformComponent>()->SetPosition(-13.5f, 28.8f, -6.4f);
	camera2->GetComponent<TransformComponent>()->SetRotation(Math::PI / 180.0f * 13.0f, Math::PI / 180.0f * 61.0f, 0.0f);

	std::shared_ptr<Object> gobber = AddObject(Object::Create("Gobber"));
	gobber->AddComponent<Model>("Model/Sample/gobber/GoblinX.obj", 4.0f);
	gobber->GetComponent<TransformComponent>()->SetPosition(-30.0f, 10.0f, 0.0f);
	gobber->GetComponent<TransformComponent>()->SetRotation(0.0f, -Math::PI / 2.0f, 0.0f);

	std::shared_ptr<Object> nano = AddObject(Object::Create("Nano"));
	nano->AddComponent<Model>("Model/Sample/nano_textured/nanosuit.obj", 1.0f);
	nano->GetComponent<TransformComponent>()->SetPosition(27.0f, -0.56f, 1.7f);
	nano->GetComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);

	std::shared_ptr<Object> sponza = AddObject(Object::Create("Sponza"));
	sponza->AddComponent<Model>("Model/Sample/sponza/sponza.obj", 1.0f / 20.0f);

	std::shared_ptr<Object> chair = AddObject(Object::Create("RoyalChair"));
	chair->AddComponent<Model>("Model/Sample/Chair/Chair.gltf", 7.0f);
	chair->GetComponent<TransformComponent>()->SetRotation(Math::PI / 2.0f, -Math::PI / 2.0f, 0.0f);
	chair->GetComponent<TransformComponent>()->SetPosition(-60.0f, 6.5f, 1.5f);

	std::shared_ptr<Object> table = AddObject(Object::Create("White Cube"));
	table->AddComponent<ColorCubeObject>();
    
    // 중력이 작용하는 물리 큐브 생성 
    std::shared_ptr<Object> physicsCube = AddObject(Object::Create("Physics Cube"));
    physicsCube->AddComponent<ColorCubeObject>();
    physicsCube->GetComponent<TransformComponent>()->SetPosition(0.0f, 20.0f, 0.0f);
    physicsCube->GetComponent<TransformComponent>()->SetScale(1.0f, 1.0f, 1.0f);
    
    // 물리 컴포넌트 추가 (질량 10, 동적 객체)
    physicsCube->AddComponent<PhysicsComponent>(10.0f, true);
    
    // 바닥 평면 생성 (정적 물리 객체)
    std::shared_ptr<Object> ground = AddObject(Object::Create("Ground Plane"));
    ground->GetComponent<TransformComponent>()->SetPosition(0.0f, 0.0f, 0.0f);
    ground->GetComponent<TransformComponent>()->SetScale(50.0f, 0.1f, 50.0f);
    
    // 정적 물리 컴포넌트 추가 (질량 0, 정적 객체)
    ground->AddComponent<PhysicsComponent>(0.0f, false);

	std::shared_ptr<Object> pointLight = AddObject(Object::Create("PointLight"));
	std::shared_ptr<PointLight> pointComponent = pointLight->AddComponent<PointLight>();
	pointLight->GetComponent<TransformComponent>()->SetPosition(0.0f, 10.0f, 0.0f);

	App::GetRenderGraph().RenderShadowCamera(*pointComponent->GetLightViewCamera()->GetComponent<Camera>());

	Scene::Initialize();
}

std::shared_ptr<Scene> SponzaScene::Create(std::string sceneName)
{
	activeScene = std::make_shared<SponzaScene>(sceneName);

	return activeScene;
}
