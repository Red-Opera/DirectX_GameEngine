#pragma once

#include "Core/Draw/Base/Drawable.h"

class CameraIndicator : public Drawable
{
public:
	CameraIndicator();

	void SetPosition(Position position) noexcept;
	void SetRotation(Quaternion rotation) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	Position position = Position::zero;
	Quaternion rotation = Quaternion::identity;
};