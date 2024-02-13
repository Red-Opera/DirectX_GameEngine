#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;	// 카메라의 Matrix를 반환함
	
	void SpawnControlWindow() noexcept;				// 카메라를 조절할 수 있는 ImGui를 생성함
	void Reset() noexcept;							// 카메라 위치 초기화

private:
	float r = 20.0f;
	float theta = 0.0f;
	float phi = 0.0f;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};