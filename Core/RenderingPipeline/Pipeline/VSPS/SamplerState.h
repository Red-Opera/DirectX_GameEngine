#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class SamplerState : public Render
	{
	public:
		// 이방성 필터링, 바이리니어 필터링, 포인트 필터링
		enum class TextureFilter { Anisotropic, Bilinear, Point, };

		SamplerState(TextureFilter textureFilter, bool useReflect, UINT slot);

		// Bindable을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;

		static std::shared_ptr<SamplerState> GetRender(
			TextureFilter textureFilter = TextureFilter::Anisotropic,
			bool useReflect = false,
			UINT slot = 0u);

		static std::string CreateID(TextureFilter textureFilter, bool useReflect, UINT slot);

		std::string GetID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		TextureFilter textureFilter;	// 텍스터 필터링 타입
		bool useReflect;				// 반사 사용 여부
		UINT slot;
	};
}