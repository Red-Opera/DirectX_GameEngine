#pragma once

#include "Core/RenderingPipeline/Render.h"

class Image;

namespace Graphic
{
	class TextureCube : public Render
	{
	public:
		TextureCube(DxGraphic& graphic, const std::string& path, UINT slot = 0);

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	private:
		std::string path;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

		UINT slot;
	};
}