#pragma once
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"

class DxGraphic;

namespace Graphic
{
	class RenderTarget;

	class DepthStencil : public Render, public BufferResource
	{
		friend RenderTarget;

	public:
		void RenderAsBuffer(DxGraphic& graphic) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, BufferResource* bufferResource) NOEXCEPTRELEASE override;
		void RenderAsBuffer(DxGraphic& graphic, RenderTarget* renderTarget) NOEXCEPTRELEASE;

		void Clear(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		DepthStencil(DxGraphic& graphic, UINT width, UINT height, bool canRenderShaderInput);
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(DxGraphic& graphic, UINT slot);
		ShaderInputDepthStencil(DxGraphic& graphic, UINT width, UINT height, UINT slot);

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