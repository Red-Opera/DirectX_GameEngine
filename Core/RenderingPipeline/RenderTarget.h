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
		void RenderAsBuffer() NOEXCEPTRELEASE override;
		void RenderAsBuffer(BufferResource* bufferResource) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DepthStencil* depthStencil) NOEXCEPTRELEASE;

		void Clear() NOEXCEPTRELEASE override;
		void Clear(const std::array<float, 4>& color) NOEXCEPTRELEASE;

		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetTargetView() const noexcept;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const noexcept;

		GraphicResource::Image ToImage() const;
		void CreateDumpy(const std::string& path) const;

	protected:
		RenderTarget(ID3D11Texture2D* texture, std::optional<UINT> face);
		RenderTarget(UINT width, UINT height);

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;

		UINT width;
		UINT height;

	private:
		void RenderAsBuffer(ID3D11DepthStencilView* depthStencilView) NOEXCEPTRELEASE;

		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> CreateStaging() const;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(UINT width, UINT height, UINT slot);

		// RenderTarget을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;


	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	class OutputOnlyRenderTarget : public RenderTarget
	{
	public:
		OutputOnlyRenderTarget(ID3D11Texture2D* texture, std::optional<UINT> face = { });

		// RenderTarget을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;
	};
}
