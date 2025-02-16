#include "stdafx.h"
#include "HorizontalBlurPass.h"

#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"

using namespace Graphic;

namespace RenderGraphNameSpace
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, DxGraphic& graphic, unsigned int width, unsigned int height)
		: PostProcessFullScreenRenderPass(std::move(name), graphic)
	{
		AddRender(PixelShader::GetRender(graphic, "Shader/PostProcessing/OutlineBlur.hlsl"));
		AddRender(ColorBlend::GetRender(graphic, false));
		AddRender(SamplerState::GetRender(graphic, SamplerState::TextureFilter::Point));

		AddRenderSink<RenderTarget>("scratchIn");
		AddRenderSink<CachingPixelConstantBufferEx>("kernel");
		AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));

		renderTarget = std::make_shared<ShaderInputRenderTarget>(graphic, width / 2, height / 2, 0u);
		AddDataProvider(DirectRenderPipelineDataProvider<RenderTarget>::Create("scratchOut", renderTarget));
	}

	void HorizontalBlurPass::Execute(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		auto buffer = direction->GetBuffer();
		buffer["isHorizontal"] = true;

		direction->SetBuffer(buffer);
		direction->SetRenderPipeline(graphic);

		PostProcessFullScreenRenderPass::Execute(graphic);
	}
}