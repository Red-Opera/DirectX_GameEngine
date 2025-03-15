#pragma once

#include "Core/RenderingPipeline/Render.h"

class Image;

namespace Graphic
{
	class OutputOnlyDepthStencil;
	class OutputOnlyRenderTarget;

	class TextureCube : public Render
	{
	public:
		TextureCube(const std::string& path, UINT slot = 0);

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	private:
		std::string path;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

		UINT slot;
	};

	class DepthTextureCube : public Render
	{
	public:
		DepthTextureCube(UINT size, UINT slot = 0);

		std::shared_ptr<OutputOnlyDepthStencil> GetDepthStencil(size_t index) const;

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>> depthStencil;

	private:
		UINT slot;
	};

	class RenderTargetTextureCube : public Render
	{
	public:
		RenderTargetTextureCube(UINT width, UINT height, UINT slot = 0, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

		std::shared_ptr<OutputOnlyRenderTarget> GetRenderTarget(size_t index) const;

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
		std::vector<std::shared_ptr<OutputOnlyRenderTarget>> renderTarget;

	private:
		UINT slot;
	};
}