#include "stdafx.h"

#include "MeshComponent.h"
#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

MeshComponent::MeshComponent(std::shared_ptr<class Object> object, std::vector<Mesh*> meshs)
    : Component(object)
{
	this->meshs = meshs;
}

void MeshComponent::Initialize()
{
    Component::Initialize();
}

void MeshComponent::Update()
{
    Component::Update();
}

void MeshComponent::CreateMesh(const Material& material, const aiMesh& mesh, float scale)
{
    meshs.push_back(std::make_unique<Mesh>(material, mesh, scale).get());
}

void MeshComponent::AddMesh(Mesh* meshPtr)
{
	meshs.push_back(std::move(meshPtr));
}

std::vector<Mesh*>& MeshComponent::GetMeshes()
{
	return meshs;
}