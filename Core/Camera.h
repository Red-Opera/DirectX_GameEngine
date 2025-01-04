#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;	// 카메라의 Matrix를 반환함
	
	void SpawnControlWindow() noexcept;				// 카메라를 조절할 수 있는 ImGui를 생성함
	void Reset() noexcept;							// 카메라 위치 초기화
	
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	DirectX::XMFLOAT3 GetPosition() const noexcept;	// 카메라 위치를 반환함

private:
	DirectX::XMFLOAT3 position;	// 카메라 위치

	// 카메라 회전
	float pitch = 0.0f;
	float yaw = 0.0f;

	static constexpr float moveSpeed = 12.0f;		// 카메라 이동 속도
	static constexpr float rotationSpeed = 0.0004f;	// 카메라 회전 속도
};