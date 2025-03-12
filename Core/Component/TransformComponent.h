#pragma once

#include "Core/Object/EngineLoop.h"

#include "Component.h"
#include "Utility/Vector.h"

#include <DirectXMath.h>
#include <string>
#include <memory>

class TransformComponent : public Component
{
public:
	TransformComponent() = default;
	virtual ~TransformComponent() = default;

	// =============================================
	// [World]
	// =============================================

	void SetPosition(Position position) noexcept;
	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetPosition(float x, float y, float z) noexcept;

	void SetRotation(Rotation rotation) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;

	void SetScale(Scale scale) noexcept;
	void SetScale(DirectX::XMFLOAT3 scale) noexcept;
	void SetScale(float x, float y, float z) noexcept;

	const Vector3 GetRight() const noexcept;
	const Vector3 GetUp() const noexcept;
	const Vector3 GetForward() const noexcept;

	// =============================================
	// [Local]
	// =============================================

	void SetLocalPosition(Position position) noexcept;
	void SetLocalPosition(float x, float y, float z) noexcept;

	void SetLocalRotation(Rotation rotation) noexcept;
	void SetLocalRotation(float roll, float pitch, float yaw) noexcept;

	void SetLocalScale(Scale scale) noexcept;
	void SetLocalScale(float x, float y, float z) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	DirectX::XMMATRIX GetLocalTransformMatrix() const noexcept;

	virtual std::string GetName() const { return "TransformComponent"; }	// 객체의 컴포넌트 이름을 반환하는 함수
	static std::string GetClassName() { return "TransformComponent"; }	// 해당 클래스의 이름을 반환하는 함수

private:
	Position position	= { 0.0f, 0.0f, 0.0f };
	Rotation rotation	= { 0.0f, 0.0f, 0.0f };
	Scale scale			= { 1.0f, 1.0f, 1.0f };

	Position localPosition	= { 0.0f, 0.0f, 0.0f };
	Rotation localRotation	= { 0.0f, 0.0f, 0.0f };
	Scale localScale		= { 1.0f, 1.0f, 1.0f };
};