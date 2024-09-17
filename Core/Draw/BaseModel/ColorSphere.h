#pragma once

#include "../Base/Drawable.h"

class ColorSphere : public Drawable
{
public:
	ColorSphere(DxGraphic& graphic, float radius);

	void SetPos(DirectX::XMFLOAT3 position) noexcept;
	
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 position = { 1.0f, 1.0f, 1.0f };
};

