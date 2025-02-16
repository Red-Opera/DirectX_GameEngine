#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class SamplerState : public Render
	{
	public:
		// �̹漺 ���͸�, ���̸��Ͼ� ���͸�, ����Ʈ ���͸�
		enum class TextureFilter { Anisotropic, Bilinear, Point, };

		SamplerState(DxGraphic& graphic, TextureFilter textureFilter, bool useReflect);

		// Bindable��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		static std::shared_ptr<SamplerState> GetRender(
			DxGraphic& graphic, 
			TextureFilter textureFilter = TextureFilter::Anisotropic,
			bool useReflect = false);

		static std::string CreateID(TextureFilter textureFilter, bool useReflect);

		std::string GetID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		TextureFilter textureFilter;	// �ؽ��� ���͸� Ÿ��
		bool useReflect;				// �ݻ� ��� ����
	};
}