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
	HorizontalBlurPass::HorizontalBlurPass(std::string name, unsigned int width, unsigned int height)
		: PostProcessFullScreenRenderPass(std::move(name))
	{
		AddRender(PixelShader::GetRender("Shader/PostProcessing/OutlineBlur.hlsl"));
		AddRender(ColorBlend::GetRender(false));
		AddRender(SamplerState::GetRender(SamplerState::TextureFilter::Point));

		AddRenderSink<RenderTarget>("scratchIn");
		AddRenderSink<CachingPixelConstantBufferEx>("kernel");
		AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));

		renderTarget = std::make_shared<ShaderInputRenderTarget>(width / 2, height / 2, 0u);
		AddDataProvider(DirectRenderPipelineDataProvider<RenderTarget>::Create("scratchOut", renderTarget));
	}

	void HorizontalBlurPass::Execute() const NOEXCEPTRELEASE
	{
		auto buffer = direction->GetBuffer();
		buffer["isHorizontal"] = true;

		direction->SetBuffer(buffer);
		direction->SetRenderPipeline();

		PostProcessFullScreenRenderPass::Execute();
	}
}