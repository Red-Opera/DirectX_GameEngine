#include "stdafx.h"
#include "RenderingPass.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"


namespace RenderGraphNameSpace
{
	RenderingPass::RenderingPass(std::string name, std::vector<std::shared_ptr<Graphic::Render>> renders)
		: RenderPass(std::move(name)), renders(std::move(renders))
	{

	}

	void RenderingPass::AddRender(std::shared_ptr<Graphic::Render> render) noexcept
	{
		renders.push_back(std::move(render));
	}

	void RenderingPass::RenderAll(DxGraphic& graphic) const noexcept
	{
		RenderBufferResources(graphic);

		for (auto& render : renders)
			render->SetRenderPipeline(graphic);
	}

	void RenderingPass::Finalize()
	{
		RenderPass::Finalize();

		if (renderTarget == nullptr && depthStencil == nullptr)
			throw RENDER_GRAPHIC_EXCEPTION("Rendering Pass [" + GetName() + "]가 Render Target 또는 Depth Stencil 중 하나가 필요함");
	}

	void RenderingPass::RenderBufferResources(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		if (renderTarget != nullptr)
			renderTarget->RenderAsBuffer(graphic, depthStencil.get());

		else
			depthStencil->RenderAsBuffer(graphic);
	}
}