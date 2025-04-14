#pragma once

#include "Core/Draw/Base/Drawable.h"

class CameraIndicator : public Drawable
{
public:
	CameraIndicator();

	void SetPosition(Position position) noexcept;
	void SetRotation(Rotation rotation) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	Position position = { 0.0f, 0.0f, 0.0f };
	Rotation rotation = { 0.0f, 0.0f, 0.0f };
};