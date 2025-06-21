#include "stdafx.h"
#include "ScreenHorizontalBlurPass.h"

#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/Exception/RenderGraphCompileException.h"

#include <sstream>

using namespace Graphic;

namespace RenderGraphNameSpace
{
    ScreenHorizontalBlurPass::ScreenHorizontalBlurPass(std::string name, unsigned int width, unsigned int height)
        : PostProcessFullScreenRenderPass(std::move(name))
    {
        if (width == 0 || height == 0)
        {
            std::ostringstream oss;
            oss << "스크린 수평 블러 패스 생성 오류: 잘못된 해상도입니다.\n"
                << "입력된 해상도: " << width << " x " << height;
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        AddRender(PixelShader::GetRender("Shader/PostProcessing/ScreenBlur.hlsl"));
        AddRender(ColorBlend::GetRender(false));
        AddRender(SamplerState::GetRender(SamplerState::TextureFilter::Point));

        AddRenderDataConsumer<RenderTarget>("scratchIn");
        AddRenderDataConsumer<CachingPixelConstantBufferEx>("kernel");
        AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction));

        renderTarget = std::make_shared<ShaderInputRenderTarget>(width / 2, height / 2, 0u);
        AddDataProvider(DirectRenderPipelineDataProvider<RenderTarget>::Create("scratchOut", renderTarget));
    }

    void ScreenHorizontalBlurPass::Execute() NOEXCEPTRELEASE
    {
        auto buffer = direction->GetBuffer();
        buffer["isHorizontal"] = true;
        direction->SetBuffer(buffer);
        direction->SetRenderPipeline();

        PostProcessFullScreenRenderPass::Execute();
    }
}
