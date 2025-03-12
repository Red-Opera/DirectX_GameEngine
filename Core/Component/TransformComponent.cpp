#include "stdafx.h"
#include "TransformComponent.h"

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
