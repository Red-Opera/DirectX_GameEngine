#pragma once

#include "../Render.h"
#include "../RenderManager.h"

namespace Graphic
{
	class PrimitiveTopology : public Render
	{
	public:
		PrimitiveTopology(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology);

		void PipeLineSet(DxGraphic& grahpic) noexcept override;

		static std::shared_ptr<PrimitiveTopology> GetRender(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology);
		static std::string CreateID(D3D11_PRIMITIVE_TOPOLOGY topology);
		std::string GetID() const noexcept override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
}