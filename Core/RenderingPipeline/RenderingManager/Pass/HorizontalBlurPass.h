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
    // 수평 방향 블러 효과를 적용하는 후처리 패스 클래스
    class HorizontalBlurPass : public PostProcessFullScreenRenderPass
    {
    public:
        // 패스 이름과 해상도로 수평 블러 패스 생성
        HorizontalBlurPass(std::string name, unsigned int width, unsigned int height);

        // 수평 블러 효과 실행
        void Execute() NOEXCEPTRELEASE override;

    private:
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction; // 블러 방향 제어 상수 버퍼
    };
}