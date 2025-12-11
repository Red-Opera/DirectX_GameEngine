#include "stdafx.h"
#include "OutlineScaleRenderGraph.h"

#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferPassClear.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/LambertianRenderPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineDrawPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineMaskPass.h"

namespace RenderGraphNameSpace
{
	OutlineScaleRenderGraph::OutlineScaleRenderGraph()
		: RenderGraph()
	{
		{
			auto pass = std::make_unique<BufferPassClear>("clearRenderTarget");
			pass->SetConsumerLinkage("buffer", "$.backbuffer");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<BufferPassClear>("clearDepthStencil");
			pass->SetConsumerLinkage("buffer", "$.masterDepth");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<LambertianRenderPass>("lambertian");
			pass->SetConsumerLinkage("renderTarget", "clearRenderTarget.renderTarget");
			pass->SetConsumerLinkage("depthStencil", "clearDepthStencil.depthStencil");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineMaskPass>("outlineMask");
			pass->SetConsumerLinkage("depthStencil", "lambertian.depthStencil");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineDrawPass>("outlineDraw");
			pass->SetConsumerLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetConsumerLinkage("depthStencil", "outlineMask.depthStencil");
			AddRenderPass(std::move(pass));
		}

		SetGlobalConsumerTarget("backbuffer", "outlineDarw.renderTarget");
		Finalize();
	}
}
