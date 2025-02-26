#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBuffer.h"

#include "../BaseModel/ColorSphere.h"

namespace RenderGraphNameSpace { class RenderGraph; }

class Camera;

class PointLight
{
public:
	PointLight(DxGraphic& graphic, DirectX::XMFLOAT3 position = { 0.0f, 10.0f, 0.0f }, float radius = 0.5f);

	void CreatePositionChangeWindow() noexcept;
	void Reset() noexcept;
	void Submit(size_t channel) const NOEXCEPTRELEASE;
	void Update(DxGraphic& graphic, DirectX::FXMMATRIX view) const noexcept;

	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

	std::shared_ptr<Camera> GetLightViewCamera() const noexcept;

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

	std::shared_ptr<Camera> viewCamera;

	PointLightConstantBuffer lightInfo;
	PointLightConstantBuffer initLightInfo;

	mutable ColorSphere mesh;
	mutable Graphic::PixelConstantBuffer<PointLightConstantBuffer> cBuffer;
};