#pragma once

#include "Core/Draw/Base/Drawable.h"

class TestTextureCube : public Drawable
{
public:
	TestTextureCube(DxGraphic& graphic, float size);

	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;

	// Drawable을(를) 통해 상속됨
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

	void SpawnControlWindow(DxGraphic& graphic) noexcept;

private:
	DirectX::XMFLOAT3 position = { 1.0f, 1.0f, 1.0f };

	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalEnabled = TRUE;
		float padding[1];
	} matConst;


	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};

