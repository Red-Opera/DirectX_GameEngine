#pragma once

#include "Core/RenderingPipeline/Render.h"
#include <array>

namespace Graphic
{
	class Rasterizer : public Render
	{
	public:
		Rasterizer(DxGraphic& graphic, bool isTwoSided);

		static std::shared_ptr<Rasterizer> GetRender(DxGraphic& graphic, bool isTwoSided);
		static std::string CreateID(bool isTwoSided);
		std::string GetID() const noexcept override;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
		bool isTwoSided;												// 레스터화기를 양면으로 사용할지 여부
	};
}