#pragma once
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"
#include "Core/Draw/Base/Image/Image.h"

class DxGraphic;

namespace Graphic
{
	class RenderTarget;

	class DepthStencil : public Render, public BufferResource
	{
		friend RenderTarget;

	public:
		enum class Usage { DepthStencil, ShadowDepth, };

		void RenderAsBuffer() NOEXCEPTRELEASE override;
		void RenderAsBuffer(BufferResource* bufferResource) NOEXCEPTRELEASE override;
		void RenderAsBuffer(RenderTarget* renderTarget) NOEXCEPTRELEASE;

		GraphicResource::Image ToImage(bool linearlize = true) const;
		void CreateDumpy(const std::string& path) const;

		UINT GetWidth() const;
		UINT GetHeight() const;

		void Clear() NOEXCEPTRELEASE override;

	protected:
		DepthStencil(UINT width, UINT height, bool canRenderShaderInput, Usage usage);
		DepthStencil(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face);

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		UINT width;
		UINT height;

	private:
		// GPU 리소스를 CPU가 읽을 수 있는 임시 리소스로 복사하여 반환 (반환 값 : CPU에서 읽을 수 있는 깊이 데이터, 원본 깊이 버퍼 정보)
		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> CreateStaging() const;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(UINT slot, Usage usage = Usage::DepthStencil);
		ShaderInputDepthStencil(UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);

		// DepthStencil을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	private:
		UINT slot;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil();
		OutputOnlyDepthStencil(UINT width, UINT height);
		OutputOnlyDepthStencil(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face);

		// DepthStencil을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;
	};
}