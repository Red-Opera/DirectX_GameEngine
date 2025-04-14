#include "stdafx.h"
#include "Scene.h"

#include "Core/App.h"
#include "Core/Window.h"
#include "Core/Camera/Camera.h"
#include "Core/Object/Object.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"
#include "Core/RenderingPipeline/RenderingChannel.h"

#include "Utility/GameTimer.h"

std::shared_ptr<Scene> Scene::activeScene = nullptr;

Scene::Scene(std::string sceneName)
	: sceneName(sceneName), sceneGraph(std::make_shared<SceneGraph>(sceneName, objects))
{
	CameraContainer::ResetCamera();
}

std::shared_ptr<Scene> Scene::Create(std::string sceneName)
{
	activeScene = std::make_shared<Scene>(sceneName);

	return std::make_shared<Scene>(sceneName);
}

std::shared_ptr<Object> Scene::AddObject(std::shared_ptr<Object> object)
{
	objects.push_back(object);
	objectIndex[object->GetName()] = (size_t)(objects.size() - 1);

	return object;
}

std::shared_ptr<Object> Scene::RemoveObject(const std::string& name)
{
	if (objectIndex.find(name) == objectIndex.end())
		return nullptr;

	size_t index = objectIndex[name];
	std::shared_ptr<Object> object = objects[index];

	objects.erase(objects.begin() + index);
	objectIndex.erase(name);

	for (size_t i = index; i < objects.size(); i++)
		objectIndex[objects[i]->GetName()] = i;

	return object;
}

std::shared_ptr<Object> Scene::RemoveObject(size_t index)
{
	if (index >= objects.size())
		return nullptr;

	std::shared_ptr<Object> object = objects[index];

	objects.erase(objects.begin() + index);
	objectIndex.erase(object->GetName());

	for (size_t i = index; i < objects.size(); i++)
		objectIndex[objects[i]->GetName()] = i;

	return object;
}

std::shared_ptr<Object> Scene::GetObject(const std::string& name)
{
	if (objectIndex.find(name) == objectIndex.end())
		return nullptr;

	return objects[objectIndex[name]];
}

std::shared_ptr<Object> Scene::GetObject(size_t index)
{
	if (index >= objects.size())
		return nullptr;

	return objects[index];
}

std::vector<std::shared_ptr<Object>> Scene::GetObjects()
{
	return objects;
}

std::string Scene::GetName() const
{
	return sceneName;
}

void Scene::CameraMoveRotation(Window& window, const GameTimer& timer, float deltaTime)
{
	auto cameraDelta = timer.DeltaTime() * cameraSpeed;

	if (!window.GetCursorEnabled())
	{
		if (window.keyBoard.IsPressed(VK_SHIFT))
			cameraDelta *= 5.0f;

		if (window.keyBoard.IsPressed('W'))
			cameras->Translate(Position::forward * cameraDelta);

		if (window.keyBoard.IsPressed('A'))
			cameras->Translate(Position::left * cameraDelta);

		if (window.keyBoard.IsPressed('S'))
			cameras->Translate(Position::back * cameraDelta);

		if (window.keyBoard.IsPressed('D'))
			cameras->Translate(Position::right * cameraDelta);

		if (window.keyBoard.IsPressed('Q'))
			cameras->Translate(Position::up * cameraDelta);

		if (window.keyBoard.IsPressed('E'))
			cameras->Translate(Position::down * cameraDelta);
	}

	while (const auto mouseDelta = window.mouse.ReadRawDelta())
	{
		if (!window.GetCursorEnabled())
			cameras->Rotate((float)mouseDelta->x, (float)mouseDelta->y);
	}
}

bool Scene::HasObject(const std::string& name)
{
	return GetObject(name) == nullptr;
}

std::shared_ptr<Scene> Scene::GetActiveScene()
{
	if (activeScene == nullptr)
		return nullptr;

	return activeScene;
}

void Scene::Initialize()
{
	for (auto& object : objects)
		object->Initialize();

	cameras.LinkTechniques(App::GetRenderGraph());
}

void Scene::BeforeFrame()
{
	for (auto& object : objects)
		object->BeforeFrame();
}

void Scene::Start()
{
	for (auto& object : objects)
		object->Start();
}

void Scene::LateStart()
{
	for (auto& object : objects)
		object->LateStart();
}

void Scene::Update()
{
	if (!CameraContainer::IsActiveCameraVaild() || !CameraContainer::IsControlCameraVaild())
		return;

	const Camera& activeCamera = CameraContainer::GetActiveCamera();

	// 활성 카메라의 뷰 행렬과 투영 행렬 구하기
	DirectX::XMMATRIX viewMatrix = activeCamera.GetMatrix();
	DirectX::XMMATRIX projMatrix = activeCamera.GetProjection();
	RenderGraphNameSpace::RenderJob::GetViewFrustum().UpdateFromMatrices(viewMatrix, projMatrix);

	App::GetRenderGraph().RenderMainCamera(cameras.GetActiveCamera());

	for (auto& object : objects)
		object->Update();

	sceneGraph->Update();

	cameras.Submit(RenderingChannel::main);
}

void Scene::LateUpdate()
{
	for (auto& object : objects)
		object->LateUpdate();

	cameras.CreateWindow();
}

void Scene::Finalize()
{
	for (auto& object : objects)
		object->Finalize();
}

void Scene::Destroy()
{
	for (auto& object : objects)
		object->Destroy();

	CameraContainer::ResetCamera();
}

void Scene::OnEnable()
{
	for (auto& object : objects)
		object->OnEnable();
}

void Scene::OnDisable()
{
	for (auto& object : objects)
		object->OnDisable();
}

void Scene::Reset()
{
	for (auto& object : objects)
		object->Reset();
}
