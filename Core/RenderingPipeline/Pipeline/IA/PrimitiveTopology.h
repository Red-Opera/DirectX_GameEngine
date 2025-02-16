#pragma once

#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	class PrimitiveTopology : public Render
	{
	public:
		PrimitiveTopology(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology);

		void SetRenderPipeline(DxGraphic& grahpic) NOEXCEPTRELEASE override;

		static std::shared_ptr<PrimitiveTopology> GetRender(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string CreateID(D3D11_PRIMITIVE_TOPOLOGY topology);
		std::string GetID() const noexcept override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
}