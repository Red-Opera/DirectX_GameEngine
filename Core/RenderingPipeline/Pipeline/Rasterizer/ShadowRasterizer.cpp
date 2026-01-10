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

		D3D11_RASTERIZER_DESC rasterizerDESC = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDESC.DepthBias = depthBias;
		rasterizerDESC.SlopeScaledDepthBias = slopeBias;
		rasterizerDESC.DepthBiasClamp = clamp;

		HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateRasterizerState(&rasterizerDESC, &rasterizerState);
		Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "해당 설정으로 그림자 레스터화기 상태 생성 실패");
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
		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->RSSetState(rasterizerState.Get()); }, ErrorCode::GRAPHICS_BindFailed, "그림자 레스터화기 상태를 렌더링 파이프라인에 설정하는데 실패했습니다.");
	}
}
