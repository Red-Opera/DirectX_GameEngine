#include "stdafx.h"
#include "ShadowRasterizer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"


namespace Graphic
{
	ShadowRasterizer::ShadowRasterizer(int depthBias, float slopeBias, float clamp)
	{
		ChangeDepthBias(depthBias, slopeBias, clamp);
	}

	void ShadowRasterizer::ChangeDepthBias(int depthBias, float slopeBias, float clamp)
	{
		this->depthBias = depthBias;
		this->slopeBias = slopeBias;
		this->clamp = clamp;

		CREATEINFOMANAGER(Window::GetDxGraphic());

		D3D11_RASTERIZER_DESC rasterizerDESC = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDESC.DepthBias = depthBias;
		rasterizerDESC.SlopeScaledDepthBias = slopeBias;
		rasterizerDESC.DepthBiasClamp = clamp;

		hr = GetDevice(Window::GetDxGraphic())->CreateRasterizerState(&rasterizerDESC, &rasterizerState);
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

	void ShadowRasterizer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->RSSetState(rasterizerState.Get()));
	}
}
