#include "stdafx.h"
#include "Object.h"

void Object::Initialize()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Initialize();
	}
}

void Object::BeforeFrame()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->BeforeFrame();
	}
}

void Object::Start()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Start();
	}
}

void Object::LateStart()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->LateStart();
	}
}

void Object::Update()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Update();
	}
}

void Object::LateUpdate()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->LateUpdate();
	}
}

void Object::Finalize()
{
	if (!isActive)
		return;

	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Finalize();
	}
}

void Object::Destroy()
{
	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Destroy();
	}
}

void Object::OnEnable()
{
	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->OnEnable();
	}
}

void Object::OnDisable()
{
	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->OnDisable();
	}
}

void Object::Reset()
{
	for (auto& compoent : components)
	{
		if (compoent.second->GetEnable())
			compoent.second->Reset();
	}
}

Object::Object()
{
	components.clear();
}

const std::vector<std::shared_ptr<Component>> Object::GetAllComponents()
{
	std::vector<std::shared_ptr<Component>> result;

	for (auto& component : components)
		result.push_back(component.second);

	return result;
}

void Object::RemoveComponent(std::string componentName)
{
	if (components.find(componentName) == components.end())
		return;

	components.erase(componentName);
}

bool Object::HasComponent(std::string componentName) const
{ 
	return components.find(componentName) != components.end(); 
}
