#pragma once

#include "Base/DrawableBase.h"

class ColorSphere : public DrawableBase<ColorSphere>
{
public:
	ColorSphere(DxGraphic& graphic, float radius);

	void Update(float deltaTime) noexcept override;
	void SetPos(DirectX::XMFLOAT3 position) noexcept;
	
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	DirectX::XMFLOAT3 position = { 1.0f, 1.0f, 1.0f };
};

