#include "stdafx.h"
#include "SceneGraphNode.h"
#include "Mesh.h"
#include "ModelBase.h"

#include "Core/Component/TransformComponent.h"
#include "Core/Object/Object.h"
#include "External/Imgui/imgui.h"

SceneGraphNode::SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform, std::shared_ptr<Object> root) NOEXCEPTRELEASE
	: id(id), meshPtrs(std::move(meshPtrs)), name(name)
{
	if (root != nullptr)
	{
		object = root;
		transformComponent = root->transform;
	}

	else
	{
		object = Object::Create(name);
		transformComponent = std::make_shared<TransformComponent>(object);
	}
}

void SceneGraphNode::Submit(size_t channel, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE
{
	// 부모를 걸쳐 변한 transform 값을 이 오브젝트를 곱하여 이 오브젝트의 World Transform 값을 구함
	const auto thisLocalTransform = transformComponent->GetLocalTransformMatrix();
	const auto thisWorldTransform = thisLocalTransform * parentWorldTransform;

	// Mesh를 그림
	for (const auto meshPtr : meshPtrs)
		meshPtr->Submit(channel, thisWorldTransform);

	// 자식도 이 노드처럼 그리도록 지시
	for (const auto& child : childPtrs)
		child->Submit(channel, thisWorldTransform);
}

void SceneGraphNode::Accept(ModelBase& modelBase)
{
	if (modelBase.push(*this))
	{
		for (auto& childPtr : childPtrs)
			childPtr->Accept(modelBase);

		modelBase.pop(*this);
	}
}

void SceneGraphNode::Accept(TechniqueBase& techniqueBase)
{
	for (auto& mashPtr : meshPtrs)
		mashPtr->Accept(techniqueBase);
}

void SceneGraphNode::ApplyWorldTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&transformComponent->GetTransformMatrix4x4(), transform);
}

const Transform& SceneGraphNode::GetTranform() const noexcept
{
	return transformComponent->GetTransform();
}

const std::shared_ptr<TransformComponent>& SceneGraphNode::GetTransformComponent() const noexcept
{
	return transformComponent;
}

int SceneGraphNode::GetID() const noexcept
{
	return id;
}

const std::vector<Mesh*>& SceneGraphNode::GetMeshPtrs() const noexcept
{
	return meshPtrs;
}

void SceneGraphNode::AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE
{
	assert(child && "추가할 자식 노드가 유효하지 않습니다.");

	child->transformComponent->SetParent(transformComponent);
	transformComponent->AddChild(child->transformComponent);

	child->object->SetTransformComponent(child->transformComponent);

	childPtrs.push_back(std::move(child));
}