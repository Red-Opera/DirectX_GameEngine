#pragma once

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/VSPS/ConstantBuffer.h"

#include "../ColorSphere.h"

class PointLight
{
public:
	PointLight(DxGraphic& graphic, float radius = 0.5f);

	void CreatePositionChangeWindow() noexcept;
	void Reset() noexcept;
	void Draw(DxGraphic& graphic) const noexcept(!_DEBUG);
	void Bind(DxGraphic& graphic, DirectX::FXMMATRIX view) const noexcept;

private:
	struct PointLightConstantBuffer
	{
		alignas(16) DirectX::XMFLOAT3 position;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};

	PointLightConstantBuffer lightInfo;
	mutable ColorSphere mesh;
	mutable PixelConstantBuffer<PointLightConstantBuffer> cBuffer;
};