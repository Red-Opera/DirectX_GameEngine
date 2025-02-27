#pragma once
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"

#include <optional>

class DxGraphic;
class Image;

// 렌더링 한 결과를 특정 텍스처에 저장하고 후처리나 다른 셰이더를 적용하기 위한 클래스
namespace Graphic
{
	class DepthStencil;

	class RenderTarget : public Render, public BufferResource
	{
	public:
		void RenderAsBuffer(DxGraphic& graphic) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, BufferResource* bufferResource) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, DepthStencil* depthStencil) NOEXCEPTRELEASE;

		void Clear(DxGraphic& graphic) NOEXCEPTRELEASE override;
		void Clear(DxGraphic& graphic, const std::array<float, 4>& color) NOEXCEPTRELEASE;

		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;

		GraphicResource::Image ToImage(DxGraphic& graphic) const;
		void CreateDumpy(DxGraphic& graphic, const std::string& path) const;

	protected:
		RenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture, std::optional<UINT> face);
		RenderTarget(DxGraphic& graphic, UINT width, UINT height);

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;

		UINT width;
		UINT height;

	private:
		void RenderAsBuffer(DxGraphic& graphic, ID3D11DepthStencilView* depthStencilView) NOEXCEPTRELEASE;

		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> CreateStaging(DxGraphic& graphic) const;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(DxGraphic& graphic, UINT width, UINT height, UINT slot);

		// RenderTarget을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;


	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	class OutputOnlyRenderTarget : public RenderTarget
	{
	public:
		OutputOnlyRenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture, std::optional<UINT> face = { });

		// RenderTarget을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;
	};
}
