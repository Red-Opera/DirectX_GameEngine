#include "stdafx.h"
#include "SceneGraphNode.h"
#include "Mesh.h"
#include "ModelBase.h"

#include "External/Imgui/imgui.h"

SceneGraphNode::SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE
	: id(id), meshPtrs(std::move(meshPtrs)), name(name)
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&this->worldTransform, DirectX::XMMatrixIdentity());
}

void SceneGraphNode::Submit(size_t channel, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE
{
	// �θ� ���� ���� transform ���� �� ������Ʈ�� ���Ͽ� �� ������Ʈ�� World Transform ���� ����
	const auto thisWorldTransform =
		DirectX::XMLoadFloat4x4(&worldTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		parentWorldTransform;

	// Mesh�� �׸�
	for (const auto meshPtr : meshPtrs)
		meshPtr->Submit(channel, thisWorldTransform);

	// �ڽĵ� �� ���ó�� �׸����� ����
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

void SceneGraphNode::ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&worldTransform, transform);
}

const DirectX::XMFLOAT4X4& SceneGraphNode::GetTranform() const noexcept
{
	return worldTransform;
}

int SceneGraphNode::GetID() const noexcept
{
	return id;
}

void SceneGraphNode::AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE
{
	assert(child && "�߰��� �ڽ� ��尡 ��ȿ���� �ʽ��ϴ�.");

	childPtrs.push_back(std::move(child));
}