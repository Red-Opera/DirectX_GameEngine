#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class SamplerState : public Render
	{
	public:
		// �̹漺 ���͸�, ���̸��Ͼ� ���͸�, ����Ʈ ���͸�
		enum class TextureFilter { Anisotropic, Bilinear, Point, };

		SamplerState(TextureFilter textureFilter, bool useReflect, UINT slot);

		// Bindable��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

		static std::shared_ptr<SamplerState> GetRender(
			TextureFilter textureFilter = TextureFilter::Anisotropic,
			bool useReflect = false,
			UINT slot = 0u);

		static std::string CreateID(TextureFilter textureFilter, bool useReflect, UINT slot);

		std::string GetID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		TextureFilter textureFilter;	// �ؽ��� ���͸� Ÿ��
		bool useReflect;				// �ݻ� ��� ����
		UINT slot;
	};
}