#pragma once

#include "PostProcessFullScreenRenderPass.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

class DxGraphic;

namespace Graphic
{
    class PixelShader;
    class RenderTarget;
}

namespace RenderGraphNameSpace
{
    // 전체 화면 수직 블러를 적용하는 패스
    class ScreenVerticalBlurPass : public PostProcessFullScreenRenderPass
    {
    public:
        ScreenVerticalBlurPass(std::string name);
        void Execute() NOEXCEPTRELEASE override;

    private:
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction;
    };
}
