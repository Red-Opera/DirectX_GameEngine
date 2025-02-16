#include "stdafx.h"
#include "OutlineScaleRenderGraph.h"

#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferPassClear.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/LambertianRenderPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineDrawPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineMaskPass.h"

namespace RenderGraphNameSpace
{
	OutlineScaleRenderGraph::OutlineScaleRenderGraph(DxGraphic& graphic)
		: RenderGraph(graphic)
	{
		{
			auto pass = std::make_unique<BufferPassClear>("clearRenderTarget");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<BufferPassClear>("clearDepthStencil");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<LambertianRenderPass>(graphic, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRenderTarget.renderTarget");
			pass->SetSinkLinkage("depthStencil", "clearDepthStencil.depthStencil");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineMaskPass>(graphic, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineDrawPass>(graphic, "outlineDraw");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			AddRenderPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "outlineDarw.renderTarget");
		Finalize();
	}
}
