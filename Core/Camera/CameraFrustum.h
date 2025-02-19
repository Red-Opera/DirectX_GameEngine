#pragma once

#include "Core/Draw/Base/Drawable.h"

class CameraFrustum : public Drawable
{
public:
	CameraFrustum(DxGraphic& graphic, float width, float height, float nearZ, float farZ);

	void SetVertices(DxGraphic& graphic, float width, float height, float nearZ, float farZ);
	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
};