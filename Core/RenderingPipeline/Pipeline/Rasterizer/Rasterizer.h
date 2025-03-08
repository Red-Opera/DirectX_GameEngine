#pragma once

#include "Core/RenderingPipeline/Render.h"
#include <array>

namespace Graphic
{
	class Rasterizer : public Render
	{
	public:
		Rasterizer(bool isTwoSided);

		static std::shared_ptr<Rasterizer> GetRender(bool isTwoSided);
		static std::string CreateID(bool isTwoSided);
		std::string GetID() const noexcept override;

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
		bool isTwoSided;												// ������ȭ�⸦ ������� ������� ����
	};
}