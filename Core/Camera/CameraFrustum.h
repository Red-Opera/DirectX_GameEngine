#pragma once

#include "Core/Draw/Base/Drawable.h"

class CameraFrustum : public Drawable
{
public:
	CameraFrustum(float width, float height, float nearZ, float farZ);

	void SetVertices(float width, float height, float nearZ, float farZ);
	void SetPosition(Position position) noexcept;
	void SetRotation(Rotation rotation) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	Position position = { 0.0f, 0.0f, 0.0f };
	Rotation rotation = { 0.0f, 0.0f, 0.0f };
};