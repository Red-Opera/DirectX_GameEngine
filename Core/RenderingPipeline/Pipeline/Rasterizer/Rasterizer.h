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

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
		bool isTwoSided;												// ������ȭ�⸦ ������� ������� ����
	};
}