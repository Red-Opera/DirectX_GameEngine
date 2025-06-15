#include "stdafx.h"
#include "SponzaScene.h"

#include "Core/App.h"
#include "Core/Camera/Camera.h"
#include "Core/Component/PhysicsComponent.h"
#include "Core/Draw/Light/PointLight.h"
#include "Core/Draw/Model.h"
#include "Core/Draw/Object/ColorCubeObject.h"
#include "Core/Draw/Object/ColorSphereObject.h"
#include "Core/Draw/Object/ColorConeObject.h"
#include "Core/Draw/Object/ColorCylinderObject.h"
#include "Core/Draw/Object/ColorPlaneObject.h"
#include "Core/Draw/Object/TextureCubeObject.h"

SponzaScene::SponzaScene(std::string sceneName) : Scene(sceneName)
{

}

void SponzaScene::Initialize()
{
    std::shared_ptr<Object> camera = AddObject(Object::Create("Camera"));
    cameras.AddCamera(camera->AddComponent<Camera>());
    camera->GetComponent<TransformComponent>()->SetPosition(-22.0f, 4.0f, 0.0f);
    camera->GetComponent<TransformComponent>()->SetRotationFromEuler(0.0f, Math::PI / 2.0f, 0.0f);

    std::shared_ptr<Object> camera2 = AddObject(Object::Create("Camera2"));
    cameras.AddCamera(camera2->AddComponent<Camera>());
    camera2->GetComponent<TransformComponent>()->SetPosition(-13.5f, 28.8f, -6.4f);
    camera2->GetComponent<TransformComponent>()->SetRotationFromEuler(Math::PI / 180.0f * 13.0f, Math::PI / 180.0f * 61.0f, 0.0f);

    // Gobber 모델 - 동적 메시 콜라이더 적용
    std::shared_ptr<Object> gobber = AddObject(Object::Create("Gobber"));
    gobber->AddComponent<Model>("Model/Sample/gobber/GoblinX.obj", 4.0f);
    gobber->GetComponent<TransformComponent>()->SetPosition(-30.0f, 10.0f, 0.0f);
    gobber->GetComponent<TransformComponent>()->SetRotationFromEuler(0.0f, -Math::PI / 2.0f, 0.0f);
    
    // Gobber에 물리 컴포넌트 추가 (질량 50, 동적 객체)
    auto gobberPhysics = gobber->AddComponent<PhysicsComponent>(50.0f, true);
    gobberPhysics->SetColliderType(ColliderType::ConvexMesh); // 동적 객체이므로 ConvexMesh 사용
    gobberPhysics->SetMaterial(0.6f, 0.4f, 0.3f); // 정적마찰, 동적마찰, 반발력
    gobberPhysics->SetGravity(true);

    // Nano suit 모델 - 동적 메시 콜라이더 적용
    std::shared_ptr<Object> nano = AddObject(Object::Create("Nano"));
    nano->AddComponent<Model>("Model/Sample/nano_textured/nanosuit.obj", 1.0f);
    nano->GetComponent<TransformComponent>()->SetPosition(27.0f, 8.0f, 1.7f); // 높이를 올려서 떨어지게 함
    nano->GetComponent<TransformComponent>()->SetRotationFromEuler(0.0f, Math::PI / 2.0f, 0.0f);
    
    // Nano에 물리 컴포넌트 추가 (질량 30, 동적 객체)
    auto nanoPhysics = nano->AddComponent<PhysicsComponent>(30.0f, true);
    nanoPhysics->SetColliderType(ColliderType::ConvexMesh);
    nanoPhysics->SetMaterial(0.5f, 0.3f, 0.4f);
    nanoPhysics->SetGravity(true);

    // Sponza 건물 - 정적 메시 콜라이더 적용 (바닥 및 벽면 충돌용)
    std::shared_ptr<Object> sponza = AddObject(Object::Create("Sponza"));
    sponza->AddComponent<Model>("Model/Sample/sponza/sponza.obj", 1.0f / 20.0f);
    
    // Sponza에 정적 물리 컴포넌트 추가 (질량 0, 정적 객체)
    auto sponzaPhysics = sponza->AddComponent<PhysicsComponent>(0.0f, false);
    sponzaPhysics->SetColliderType(ColliderType::TriangleMesh); // 정적 객체이므로 TriangleMesh 사용 (더 정확한 충돌)
    sponzaPhysics->SetMaterial(0.8f, 0.6f, 0.1f); // 돌/콘크리트 같은 재질

    // 의자 모델 - 정적 메시 콜라이더 적용
    std::shared_ptr<Object> chair = AddObject(Object::Create("RoyalChair"));
    chair->AddComponent<Model>("Model/Sample/Chair/Chair.gltf", 7.0f);
    chair->GetComponent<TransformComponent>()->SetRotationFromEuler(Math::PI / 2.0f, -Math::PI / 2.0f, 0.0f);
    chair->GetComponent<TransformComponent>()->SetPosition(-60.0f, 6.5f, 1.5f);
    
    // 의자에 정적 물리 컴포넌트 추가 (질량 0, 정적 객체)
    auto chairPhysics = chair->AddComponent<PhysicsComponent>(0.0f, false);
    chairPhysics->SetColliderType(ColliderType::TriangleMesh); // 정확한 의자 형태 충돌을 위해 TriangleMesh 사용
    chairPhysics->SetMaterial(0.7f, 0.5f, 0.2f); // 나무 재질

    // 중력이 작용하는 물리 큐브 생성 
    std::shared_ptr<Object> physicsCube = AddObject(Object::Create("Physics Cube"));
    physicsCube->AddComponent<ColorCubeObject>();
    physicsCube->GetComponent<TransformComponent>()->SetPosition(0.0f, 15.0f, 0.0f);
    physicsCube->GetComponent<TransformComponent>()->SetScale(1.0f, 1.0f, 1.0f);

    // 물리 컴포넌트 추가 (질량 10, 동적 객체)
    auto cubePhysics = physicsCube->AddComponent<PhysicsComponent>(10.0f, true);
    cubePhysics->SetMaterial(0.6f, 0.4f, 0.5f);

    std::shared_ptr<Object> physicsSphere = AddObject(Object::Create("Physics Sphere"));
    physicsSphere->AddComponent<ColorSphereObject>();
    auto spherePhysics = physicsSphere->AddComponent<PhysicsComponent>(10.0f, true);
    spherePhysics->SetMaterial(0.4f, 0.2f, 0.8f); // 탄성이 좋은 구
    physicsSphere->GetComponent<TransformComponent>()->SetPosition(5.0f, 15.0f, 3.0f);

    std::shared_ptr<Object> physicsCone = AddObject(Object::Create("Physics Cone"));
    physicsCone->AddComponent<ColorConeObject>();
    auto conePhysics = physicsCone->AddComponent<PhysicsComponent>(10.0f, true);
    conePhysics->SetMaterial(0.5f, 0.3f, 0.4f);
    physicsCone->GetComponent<TransformComponent>()->SetPosition(-5.0f, 15.0f, -3.0f);

    std::shared_ptr<Object> cylinder = AddObject(Object::Create("Physics Cylinder"));
    cylinder->AddComponent<ColorCylinderObject>();
    auto cylinderPhysics = cylinder->AddComponent<PhysicsComponent>(10.0f, true);
    cylinderPhysics->SetMaterial(0.6f, 0.4f, 0.3f);
    cylinder->GetComponent<TransformComponent>()->SetPosition(3.0f, 15.0f, 0.0f);

    std::shared_ptr<Object> plane = AddObject(Object::Create("Physics Plane"));
    plane->AddComponent<ColorPlaneObject>();
    auto planePhysics = plane->AddComponent<PhysicsComponent>(5.0f, true);
    planePhysics->SetMaterial(0.5f, 0.3f, 0.6f);
    plane->GetComponent<TransformComponent>()->SetPosition(-3.0f, 15.0f, 0.0f);

    // 바닥 평면 생성 (정적 물리 객체)
    std::shared_ptr<Object> ground = AddObject(Object::Create("Ground Plane"));
    ground->AddComponent<ColorCubeObject>();
    ground->GetComponent<TransformComponent>()->SetPosition(0.0f, -0.5f, 0.0f); // 약간 아래로 이동
    ground->GetComponent<TransformComponent>()->SetScale(50.0f, 1.0f, 50.0f); // 더 큰 바닥면 생성
    ground->GetComponent<ColorCubeObject>()->SetColor(GraphicResource::Image::Color(0, 255, 0));

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
