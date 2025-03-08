#pragma once

#include "Core/Draw/Base/Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(float size, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.0f });

	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;

	// Drawable을(를) 통해 상속됨
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

	void SpawnControlWindow(const std::string& name) noexcept;

private:
	DirectX::XMFLOAT3 position = { 1.5f, 3.5f, 9.0f };

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT4 color;
	} matConst;

	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};

