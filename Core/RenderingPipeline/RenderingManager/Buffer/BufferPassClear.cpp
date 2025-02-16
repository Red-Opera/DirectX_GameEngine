#include "stdafx.h"
#include "BufferPassClear.h"

#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"

namespace RenderGraphNameSpace
{
	BufferPassClear::BufferPassClear(std::string name)
		: RenderPass(std::move(name))
	{
		AddDataConsumer(DirectBufferDataConsumer<Graphic::BufferResource>::Create("buffer", buffer));
		AddDataProvider(DirectBufferPipelineDataProvider<Graphic::BufferResource>::Create("buffer", buffer));
	}

	void BufferPassClear::Execute(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		buffer->Clear(graphic);
	}
}