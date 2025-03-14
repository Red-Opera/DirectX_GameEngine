#pragma once

#include "Core/Object/EngineLoop.h"

#include "Component.h"
#include "Utility/Vector.h"

#include <DirectXMath.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

class TransformComponent : public Component, public std::enable_shared_from_this<TransformComponent>
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
	//const Vector3 GetUp() const noexcept;
	//const Vector3 GetForward() const noexcept;

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

	virtual std::string GetClassName() const { return "TransformComponent"; }	// 객체의 컴포넌트 이름을 반환하는 함수
	static std::string GetStaticClassName() { return "TransformComponent"; }	// 해당 클래스의 이름을 반환하는 함수

	// =============================================
	// [Parent, Child]
	// =============================================

	void SetParent(std::shared_ptr<TransformComponent> parent) noexcept;
	void SetParent(std::shared_ptr<Object> parent) noexcept;

	bool HasParent() const noexcept;
	void RemoveParent() noexcept;

	void AddChild(std::shared_ptr<TransformComponent> child) noexcept;
	void AddChild(std::shared_ptr<Object> child) noexcept;

	void RemoveChild(std::shared_ptr<TransformComponent> child) noexcept;
	void RemoveChild(std::shared_ptr<Object> child) noexcept;
	void RemoveChild(std::string childObjectName) noexcept;
	void RemoveChild(UINT index) noexcept;

	bool HasChild(std::shared_ptr<TransformComponent> child) const noexcept;
	bool HasChild(std::shared_ptr<Object> child) const noexcept;
	bool HasChild(std::string childObjectName) const noexcept;
	bool HasChild(UINT index) const noexcept;

	std::shared_ptr<TransformComponent> GetChild(std::shared_ptr<Object> child) noexcept;
	std::shared_ptr<TransformComponent> GetChild(std::string childObjectName) noexcept;
	std::shared_ptr<TransformComponent> GetChild(UINT index) noexcept;

	std::vector<std::shared_ptr<TransformComponent>> GetChildrens() noexcept;

	size_t GetChildCount() const noexcept;

	void UpdateTransform() noexcept;

private:
	std::unordered_map<std::string, UINT> childIndex;
	std::vector<std::shared_ptr<TransformComponent>> children;	// 자식 오브젝트를 저장하는 벡터
	std::shared_ptr<TransformComponent> parent;					// 부모 오브젝트를 저장하는 변수

	Position position	= { 0.0f, 0.0f, 0.0f };
	Rotation rotation	= { 0.0f, 0.0f, 0.0f };
	Scale scale			= { 1.0f, 1.0f, 1.0f };

	Position localPosition	= { 0.0f, 0.0f, 0.0f };
	Rotation localRotation	= { 0.0f, 0.0f, 0.0f };
	Scale localScale		= { 1.0f, 1.0f, 1.0f };
};