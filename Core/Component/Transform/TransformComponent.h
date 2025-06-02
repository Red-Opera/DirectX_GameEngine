#pragma once
#include "Transform.h" // Quaternion, Position, Scale, Transform 정의 포함

#include "Core/Object/EngineLoop.h"

#include "../Component.h"

#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class TransformComponent : public Component, public std::enable_shared_from_this<TransformComponent>
{
public:
	TransformComponent(std::shared_ptr<class Object> object);
	virtual ~TransformComponent() = default;

	// =============================================
	// [World]
	// =============================================

	void SetPosition(Position position) noexcept;
	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetPosition(float x, float y, float z) noexcept;

	Position& GetPosition() noexcept;

	// World Rotation
	void SetRotation(const Quaternion& rotation) noexcept;						// Quaternion 사용
	void SetRotationFromEuler(const Euler& eulerAngles) noexcept;				// 오일러 각도 사용
	void SetRotationFromEuler(float roll, float pitch, float yaw) noexcept;		// 오일러 각도 사용

	Quaternion& GetRotation() noexcept; // Quaternion 반환
	Vector3 GetRotationEuler() const noexcept; // 오일러 각도 반환

	void SetScale(Scale scale) noexcept;
	void SetScale(DirectX::XMFLOAT3 scale) noexcept;
	void SetScale(float x, float y, float z) noexcept;

	Scale& GetScale() noexcept;

	const Vector3 GetRight() const noexcept;
	const Vector3 GetUp() const noexcept;
	const Vector3 GetForward() const noexcept;

	Transform& GetTransform() noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	DirectX::XMFLOAT4X4 &GetTransformMatrix4x4() noexcept;

	// =============================================
	// [Local]
	// =============================================

	void SetLocalPosition(Position position) noexcept;
	void SetLocalPosition(float x, float y, float z) noexcept;

	// Local Rotation
	void SetLocalRotation(const Quaternion& rotation) noexcept; // Quaternion 사용
	void SetLocalRotationFromEuler(const Euler& eulerAngles) noexcept; // 오일러 각도 사용
	void SetLocalRotationFromEuler(float roll, float pitch, float yaw) noexcept; // 오일러 각도 사용
	
	Quaternion& GetLocalRotation() noexcept; // Quaternion 반환
	Vector3 GetLocalRotationEuler() const noexcept; // 오일러 각도 반환

	void SetLocalScale(Scale scale) noexcept;
	void SetLocalScale(DirectX::XMFLOAT3 scale) noexcept;
	void SetLocalScale(float x, float y, float z) noexcept;

	Transform& GetLocalTransform() noexcept;

	Position& GetLocalPosition() noexcept;
	Scale& GetLocalScale() noexcept;

	DirectX::XMMATRIX GetLocalTransformMatrix() const noexcept;
	DirectX::XMFLOAT4X4& GetLocalTransformMatrix4x4() noexcept;

	virtual std::string GetClassName() const override { return "TransformComponent"; };	// 객체의 컴포넌트 이름을 반환하는 함수
	static std::string GetStaticClassName() { return "TransformComponent"; }			// 해당 클래스의 이름을 반환하는 함수

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

	void UpdateTransform() noexcept; // 월드 변환 업데이트 (필요시)

private:
	void UpdateWorldRotation() noexcept; // 로컬 회전 -> 월드 회전 업데이트
	void UpdateLocalRotation() noexcept; // 월드 회전 -> 로컬 회전 업데이트

	std::unordered_map<std::string, UINT> childIndex;
	std::vector<std::shared_ptr<TransformComponent>> children;	// 자식 오브젝트를 저장하는 벡터
	std::shared_ptr<TransformComponent> parent;					// 부모 오브젝트를 저장하는 변수

	DirectX::XMFLOAT4X4 transformMatrix;						// 오브젝트의 변환 행렬 (캐시용)

    // worldTransform과 localTransform의 rotation 멤버는 이제 Quaternion 타입입니다.
	Transform worldTransform; 
	Transform localTransform;
};