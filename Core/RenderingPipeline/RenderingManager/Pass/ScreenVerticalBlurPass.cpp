#include "stdafx.h"
#include "ScreenVerticalBlurPass.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"

using namespace Graphic;

namespace RenderGraphNameSpace
{
    ScreenVerticalBlurPass::ScreenVerticalBlurPass(std::string name)
        : PostProcessFullScreenRenderPass(std::move(name))
    {
        AddRender(PixelShader::GetRender("Shader/PostProcessing/ScreenBlur.hlsl"));
        AddRender(ColorBlend::GetRender(false));
        AddRender(SamplerState::GetRender(SamplerState::TextureFilter::Point));

        AddRenderDataConsumer<RenderTarget>("scratchIn");
        AddRenderDataConsumer<CachingPixelConstantBufferEx>("kernel");
        AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));
        AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));

        AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
    }

    void ScreenVerticalBlurPass::Execute() NOEXCEPTRELEASE
    {
        auto buffer = direction->GetBuffer();
        buffer["isHorizontal"] = false;
        direction->SetBuffer(buffer);
        direction->SetRenderPipeline();

        PostProcessFullScreenRenderPass::Execute();
    }
}
