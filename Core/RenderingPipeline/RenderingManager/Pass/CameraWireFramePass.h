#pragma once

#include "Base/RenderQueuePass.h"

#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"

class DxGraphic;

namespace RenderGraphNameSpace
{
	class CameraWireFramePass : public RenderQueuePass
	{
	public:
		CameraWireFramePass(std::string name) : RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));
			AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(Stencil::GetRender(Stencil::DrawMode::DepthReversed));
		}
	};
}