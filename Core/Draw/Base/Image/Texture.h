#pragma once
#include "Core/RenderingPipeline/Render.h"

class Material;

namespace Graphic
{
	class Texture : public Render
	{
	public:
		Texture(DxGraphic& graphic,const  std::string& path, UINT slot = 0);

		// Bindable을(를) 통해 상속됨
		void PipeLineSet(DxGraphic& graphic) noexcept override;

		static std::shared_ptr<Texture> GetRender(DxGraphic& graphic, const std::string& path, UINT slot = 0);
		static std::string CreateID(const std::string& path, UINT slot = 0);
		std::string GetID() const noexcept override;

	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	private:
		UINT slot;
	};
}