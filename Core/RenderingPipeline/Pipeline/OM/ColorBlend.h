#pragma once

#include "Core/RenderingPipeline/Render.h"

#include <array>
#include <optional>

namespace Graphic
{
	class ColorBlend : public Render
	{
	public:
		ColorBlend(DxGraphic& graphic, bool blending, std::optional<float> transparency = {});

		void SetTransparency(float transparency) NOEXCEPTRELEASE;
		float GetTransparency() const NOEXCEPTRELEASE;

		static std::shared_ptr<ColorBlend> GetRender(DxGraphic& graphic, bool blending, std::optional<float> transparency = {});
		static std::string CreateID(bool blending, std::optional<float> transparency = {});
		std::string GetID() const noexcept override;

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
		bool blending;											// ������ ������� ����

		std::optional<std::array<float, 4>> transparency;		// ����
	};
}