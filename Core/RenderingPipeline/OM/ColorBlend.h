#pragma once

#include "../Render.h"

#include <array>
#include <optional>

namespace Graphic
{
	class ColorBlend : public Render
	{
	public:
		ColorBlend(DxGraphic& graphic, bool blending, std::optional<float> transparency = {});

		void SetTransparency(float transparency) noexcept;
		float GetTransparency() const noexcept;

		static std::shared_ptr<ColorBlend> GetRender(DxGraphic& graphic, bool blending, std::optional<float> transparency = {});
		static std::string CreateID(bool blending, std::optional<float> transparency = {});
		std::string GetID() const noexcept override;

		// Render을(를) 통해 상속됨
		void PipeLineSet(DxGraphic& graphic) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
		bool blending;											// 블렌딩을 사용할지 여부

		std::optional<std::array<float, 4>> transparency;		// 투명도
	};
}