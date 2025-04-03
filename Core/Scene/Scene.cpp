#include "stdafx.h"
#include "Scene.h"

#include "Core/Draw/Model.h"

std::shared_ptr<Scene> Scene::activeScene = nullptr;

Scene::Scene(std::string sceneName)
	: sceneName(sceneName)
{

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
	objects.push_back(Object::Create("Gobber"));
	objects[0]->AddComponent<Model>("Model/Sample/gobber/GoblinX.obj", 4.0f);
	objects[0]->GetComponent<TransformComponent>()->SetPosition(-30.0f, 10.0f, 0.0f);
	objects[0]->GetComponent<TransformComponent>()->SetRotation(0.0f, -Math::PI / 2.0f, 0.0f);

	objects.push_back(Object::Create("Nano"));
	objects[1]->AddComponent<Model>("Model/Sample/nano_textured/nanosuit.obj", 1.0f);
	objects[1]->AddComponent<TransformComponent>()->SetPosition(27.0f, -0.56f, 1.7f);
	objects[1]->AddComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);

	objects.push_back(Object::Create("Sponza"));
	objects[2]->AddComponent<Model>("Model/Sample/sponza/sponza.obj", 1.0f / 20.0f);

	for (auto& object : objects)
		object->Initialize();
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
	for (auto& object : objects)
		object->Update();
}

void Scene::LateUpdate()
{
	for (auto& object : objects)
		object->LateUpdate();
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