#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;	// ī�޶��� Matrix�� ��ȯ��
	
	void SpawnControlWindow() noexcept;				// ī�޶� ������ �� �ִ� ImGui�� ������
	void Reset() noexcept;							// ī�޶� ��ġ �ʱ�ȭ
	
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	DirectX::XMFLOAT3 GetPosition() const noexcept;	// ī�޶� ��ġ�� ��ȯ��

private:
	DirectX::XMFLOAT3 position;	// ī�޶� ��ġ

	// ī�޶� ȸ��
	float pitch = 0.0f;
	float yaw = 0.0f;

	static constexpr float moveSpeed = 12.0f;		// ī�޶� �̵� �ӵ�
	static constexpr float rotationSpeed = 0.0004f;	// ī�޶� ȸ�� �ӵ�
};