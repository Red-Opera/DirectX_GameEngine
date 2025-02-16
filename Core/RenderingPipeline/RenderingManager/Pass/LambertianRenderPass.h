#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "../../RenderGraph/PipelineDataProvider.h"
#include "../../RenderGraph/PipelineDataConsumer.h"

#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"

#include <vector>

class DxGraphic;

namespace RenderGraphNameSpace
{
	class LambertianRenderPass : public RenderQueuePass
	{
	public:
		LambertianRenderPass(DxGraphic& graphic, std::string name) : RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<Graphic::DepthStencil>::Create("depthStencil", depthStencil));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::Off));
		}
	};
}