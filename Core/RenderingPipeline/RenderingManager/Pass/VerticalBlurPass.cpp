#include "stdafx.h"
#include "VerticalBlurPass.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"

using namespace Graphic;

namespace RenderGraphNameSpace
{
	VerticalBlurPass::VerticalBlurPass(std::string name, DxGraphic& graphic)
		: PostProcessFullScreenRenderPass(std::move(name), graphic)
	{
		using namespace Graphic;

		AddRender(PixelShader::GetRender(graphic, "Shader/PostProcessing/OutlineBlur.hlsl"));
		AddRender(ColorBlend::GetRender(graphic, true));
		AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::Mask));
		AddRender(SamplerState::GetRender(graphic, SamplerState::TextureFilter::Bilinear, true));

		AddRenderSink<RenderTarget>("scratchIn");
		AddRenderSink<CachingPixelConstantBufferEx>("kernel");

		AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));
		AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));
		AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));

		AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
		AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));
	}

	void VerticalBlurPass::Execute(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		auto buffer = direction->GetBuffer();
		buffer["isHorizontal"] = false;

		direction->SetBuffer(buffer);
		direction->SetRenderPipeline(graphic);

		PostProcessFullScreenRenderPass::Execute(graphic);
	}
}