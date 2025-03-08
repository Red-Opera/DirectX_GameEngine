#include "stdafx.h"
#include "PrimitiveTopology.h"

#include "Core/Window.h"

namespace Graphic
{
	PrimitiveTopology::PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) : topology(topology)
	{

	}

	void PrimitiveTopology::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->IASetPrimitiveTopology(topology));
	}

	std::shared_ptr<PrimitiveTopology> PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		return RenderManager::GetRender<PrimitiveTopology>(topology);
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