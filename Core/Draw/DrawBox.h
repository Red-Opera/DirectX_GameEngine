#pragma once
#include "TestObject.h"

#include "Core/RenderingPipeline/VSPS/ConstantBuffer.h"

class DrawBox : public TestObject<DrawBox>
{
public:
	DrawBox(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material);

	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	bool CreateControlWindow(int id, DxGraphic& graphic) noexcept;

private:
	void SyncMaterial(DxGraphic& graphic) noexcept(!_DEBUG);

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;

		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} materialConstants;

	using MaterialConstantBuffer = PixelConstantBuffer<PSMaterialConstant>;

	DirectX::XMFLOAT3X3 transformMatrix;
};

