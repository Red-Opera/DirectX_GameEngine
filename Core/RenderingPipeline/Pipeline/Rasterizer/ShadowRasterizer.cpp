#include "stdafx.h"
#include "ShadowRasterizer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"


namespace Graphic
{
	ShadowRasterizer::ShadowRasterizer(DxGraphic& graphic, int depthBias, float slopeBias, float clamp)
	{
		ChangeDepthBias(graphic, depthBias, slopeBias, clamp);
	}

	void ShadowRasterizer::ChangeDepthBias(DxGraphic& graphic, int depthBias, float slopeBias, float clamp)
	{
		this->depthBias = depthBias;
		this->slopeBias = slopeBias;
		this->clamp = clamp;

		CREATEINFOMANAGER(graphic);

		D3D11_RASTERIZER_DESC rasterizerDESC = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDESC.DepthBias = depthBias;
		rasterizerDESC.SlopeScaledDepthBias = slopeBias;
		rasterizerDESC.DepthBiasClamp = clamp;

		hr = GetDevice(graphic)->CreateRasterizerState(&rasterizerDESC, &rasterizerState);
		GRAPHIC_THROW_INFO(hr);
	}

	int ShadowRasterizer::GetDepthBias() const
	{
		return depthBias;
	}

	float ShadowRasterizer::GetSlopeBias() const
	{
		return slopeBias;
	}

	float ShadowRasterizer::GetClamp() const
	{
		return clamp;
	}

	void ShadowRasterizer::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->RSSetState(rasterizerState.Get()));
	}
}
