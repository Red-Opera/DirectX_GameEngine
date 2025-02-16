#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBuffer.h"

#include "../BaseModel/ColorSphere.h"

namespace RenderGraphNameSpace { class RenderGraph; }

class PointLight
{
public:
	PointLight(DxGraphic& graphic, float radius = 0.5f);

	void CreatePositionChangeWindow() noexcept;
	void Reset() noexcept;
	void Submit() const NOEXCEPTRELEASE;
	void Update(DxGraphic& graphic, DirectX::FXMMATRIX view) const noexcept;

	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

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
	mutable Graphic::PixelConstantBuffer<PointLightConstantBuffer> cBuffer;
};