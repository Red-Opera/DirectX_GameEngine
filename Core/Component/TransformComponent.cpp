#include "stdafx.h"
#include "TransformComponent.h"

#include "Core/Object/Object.h"

void TransformComponent::SetPosition(Position position) noexcept
{
	this->position = position;
}

void TransformComponent::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = { position.x, position.y, position.z };
}

void TransformComponent::SetPosition(float x, float y, float z) noexcept
{
	this->position = { x, y, z };
}

void TransformComponent::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->rotation = { roll, pitch, yaw };
}

void TransformComponent::SetScale(Scale scale) noexcept
{
	this->scale = scale;
}

void TransformComponent::SetScale(DirectX::XMFLOAT3 scale) noexcept
{
	this->scale = { scale.x, scale.y, scale.z };
}

void TransformComponent::SetScale(float x, float y, float z) noexcept
{
	this->scale = { x, y, z };
}

const Vector3 TransformComponent::GetRight() const noexcept
{
	// TODO
	return Vector3();
}

void TransformComponent::SetRotation(Rotation rotation) noexcept
{
	this->rotation = rotation;
}

void TransformComponent::SetRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	this->rotation = { rotation.x, rotation.y, rotation.z };
}

void TransformComponent::SetLocalPosition(Position position) noexcept
{
	this->localPosition = position;
}

void TransformComponent::SetLocalPosition(float x, float y, float z) noexcept
{
	this->localPosition = { x, y, z };
}

void TransformComponent::SetLocalRotation(float roll, float pitch, float yaw) noexcept
{
	this->localRotation = { roll, pitch, yaw };
}

void TransformComponent::SetLocalScale(Scale scale) noexcept
{
	this->localScale = scale;
}

void TransformComponent::SetLocalScale(float x, float y, float z) noexcept
{
	this->localScale = { x, y, z };
}

void TransformComponent::SetLocalRotation(Rotation rotation) noexcept
{
	this->localRotation = rotation;
}

DirectX::XMMATRIX TransformComponent::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX TransformComponent::GetLocalTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(localRotation.x, localRotation.y, localRotation.z) *
		DirectX::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);
}

void TransformComponent::SetParent(std::shared_ptr<TransformComponent> parent) noexcept
{
	this->parent = parent;

	parent->children.push_back(this->shared_from_this());
}

void TransformComponent::SetParent(std::shared_ptr<Object> parent) noexcept
{
	std::shared_ptr<TransformComponent> parentComponent = parent->GetComponent<TransformComponent>();

	this->parent = parentComponent;

	parentComponent->children.push_back(this->shared_from_this());
}

bool TransformComponent::HasParent() const noexcept
{
	return parent != nullptr;
}

void TransformComponent::RemoveParent() noexcept
{
	if (parent == nullptr)
		return;

	parent->RemoveChild(this->shared_from_this());
	parent = nullptr;
}

void TransformComponent::AddChild(std::shared_ptr<TransformComponent> child) noexcept
{
	children.push_back(child);
	childIndex[child->GetObject()->GetName()] = ((UINT)children.size() - (UINT)1);

	child->parent = this->shared_from_this();
}

void TransformComponent::AddChild(std::shared_ptr<Object> child) noexcept
{
	std::shared_ptr<TransformComponent> childComponent = child->GetComponent<TransformComponent>();

	children.push_back(childComponent);
	childIndex[child->GetName()] = ((UINT)children.size() - (UINT)1);

	childComponent->parent = this->shared_from_this();
}

void TransformComponent::RemoveChild(std::shared_ptr<TransformComponent> child) noexcept
{
	auto iter = childIndex.find(child->GetObject()->GetName());

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);
	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

void TransformComponent::RemoveChild(UINT index) noexcept
{
	if (index >= children.size())
		return;

	children.erase(children.begin() + index);
}

void TransformComponent::RemoveChild(std::shared_ptr<Object> child) noexcept
{
	auto iter = childIndex.find(child->GetName());

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);

	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

void TransformComponent::RemoveChild(std::string childObjectName) noexcept
{
	auto iter = childIndex.find(childObjectName);

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);
	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

bool TransformComponent::HasChild(std::shared_ptr<TransformComponent> child) const noexcept
{
	auto iter = childIndex.find(child->GetObject()->GetName());

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(std::shared_ptr<Object> child) const noexcept
{
	auto iter = childIndex.find(child->GetName());

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(std::string childObjectName) const noexcept
{
	auto iter = childIndex.find(childObjectName);

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(UINT index) const noexcept
{
	return index < children.size();
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::shared_ptr<Object> child) noexcept
{
	if (HasChild(child) == false)
		return nullptr;

	return children[childIndex[child->GetName()]];
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::string childObjectName) noexcept
{
	if (HasChild(childObjectName) == false)
		return nullptr;

	return children[childIndex[childObjectName]];
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(UINT index) noexcept
{
	if (index >= children.size())
		return nullptr;

	return children[index];
}

std::vector<std::shared_ptr<TransformComponent>> TransformComponent::GetChildrens() noexcept
{
	return children;
}

size_t TransformComponent::GetChildCount() const noexcept
{
	return children.size();
}

void TransformComponent::UpdateTransform() noexcept
{
}
