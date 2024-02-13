#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;	// ī�޶��� Matrix�� ��ȯ��
	
	void SpawnControlWindow() noexcept;				// ī�޶� ������ �� �ִ� ImGui�� ������
	void Reset() noexcept;							// ī�޶� ��ġ �ʱ�ȭ

private:
	float r = 20.0f;
	float theta = 0.0f;
	float phi = 0.0f;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};