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

		void RenderAsBuffer(DxGraphic& graphic) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, BufferResource* bufferResource) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, RenderTarget* renderTarget) NOEXCEPTRELEASE;

		GraphicResource::Image ToImage(DxGraphic& graphic, bool linearlize = true) const;

		UINT GetWidth() const;
		UINT GetHeight() const;

		void Clear(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		DepthStencil(DxGraphic& graphic, UINT width, UINT height, bool canRenderShaderInput, Usage usage);

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		UINT width;
		UINT height;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(DxGraphic& graphic, UINT slot, Usage usage = Usage::DepthStencil);
		ShaderInputDepthStencil(DxGraphic& graphic, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);

		// DepthStencil을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	private:
		UINT slot;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(DxGraphic& graphic);
		OutputOnlyDepthStencil(DxGraphic& graphic, UINT width, UINT height);

		// DepthStencil을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;
	};
}