#pragma once
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"

class DxGraphic;

// ������ �� ����� Ư�� �ؽ�ó�� �����ϰ� ��ó���� �ٸ� ���̴��� �����ϱ� ���� Ŭ����
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

	protected:
		RenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture);
		RenderTarget(DxGraphic& graphic, UINT width, UINT height);

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;

		UINT width;
		UINT height;

	private:
		void RenderAsBuffer(DxGraphic& graphic, ID3D11DepthStencilView* depthStencilView) NOEXCEPTRELEASE;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(DxGraphic& graphic, UINT width, UINT height, UINT slot);

		// RenderTarget��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend DxGraphic;

	public:
		// RenderTarget��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	private:
		OutputOnlyRenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture);
	};
}
