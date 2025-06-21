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
    // 전체 화면 수평 블러를 적용하는 패스
    class ScreenHorizontalBlurPass : public PostProcessFullScreenRenderPass
    {
    public:
        ScreenHorizontalBlurPass(std::string name, unsigned int width, unsigned int height);
        void Execute() NOEXCEPTRELEASE override;

    private:
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction;
    };
}
