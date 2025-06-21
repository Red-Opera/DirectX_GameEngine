#include "stdafx.h"
#include "FinalPostProcessPass.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"

using namespace Graphic;

namespace RenderGraphNameSpace
{
    FinalPostProcessPass::FinalPostProcessPass(std::string name, unsigned int width, unsigned int height)
        : PostProcessFullScreenRenderPass(std::move(name))
    {
        AddRender(PixelShader::GetRender("Shader/PostProcessing/ScreenBlur.hlsl"));
        AddRender(ColorBlend::GetRender(false));
        AddRender(SamplerState::GetRender(SamplerState::TextureFilter::Point));

        AddRenderSink<RenderTarget>("renderTargetIn");
        AddRenderSink<CachingPixelConstantBufferEx>("kernel");
        AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));

        renderTarget = std::make_shared<ShaderInputRenderTarget>(width, height, 0u);
        AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTargetOut", renderTarget));
    }

    void FinalPostProcessPass::Execute() NOEXCEPTRELEASE
    {
        auto buffer = direction->GetBuffer();
        buffer["isHorizontal"] = false;
        direction->SetBuffer(buffer);
        direction->SetRenderPipeline();

        PostProcessFullScreenRenderPass::Execute();
    }
}
