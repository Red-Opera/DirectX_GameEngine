#include "stdafx.h"
#include "PrimitiveTopology.h"

#include "../RenderManager.h"

namespace Graphic
{
	PrimitiveTopology::PrimitiveTopology(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology) : topology(topology)
	{

	}

	void PrimitiveTopology::PipeLineSet(DxGraphic& grahpic) noexcept
	{
		GetDeviceContext(grahpic)->IASetPrimitiveTopology(topology);
	}

	std::shared_ptr<PrimitiveTopology> PrimitiveTopology::GetRender(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		return RenderManager::GetRender<PrimitiveTopology>(graphic, topology);
	}

	std::string PrimitiveTopology::CreateID(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		using namespace std::string_literals;

		return typeid(PrimitiveTopology).name() + "#"s + std::to_string(topology);
	}

	std::string PrimitiveTopology::GetID() const noexcept
	{
		return CreateID(topology);
	}
}