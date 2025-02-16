#pragma once
#include "Core/RenderingPipeline/Render.h"

#include <map>

class Image;

namespace Graphic
{
	class Texture : public Render
	{
	public:
		Texture(DxGraphic& graphic,const  std::string& path, UINT slot = 0);

		// Bindable을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		static std::shared_ptr<Texture> GetRender(DxGraphic& graphic, const std::string& path, UINT slot = 0);
		static std::string CreateID(const std::string& path, UINT slot = 0);
		std::string GetID() const noexcept override;
		bool HasAlpha() const noexcept;

	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

		bool hasAlpha = false;

	private:
		static UINT CountMipLevels(UINT width, UINT height) noexcept;
		static std::unordered_map<std::string, std::unique_ptr<GraphicResource::Image>> imageCache;

		UINT slot;
	};
}