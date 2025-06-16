#pragma once

#include "Base/RenderingPass.h"

class DxGraphic;

namespace RenderGraphNameSpace
{
    // 전체 화면 후처리 효과를 위한 기본 렌더 패스 클래스
    class PostProcessFullScreenRenderPass : public RenderingPass
    {
    public:
        // 패스 이름으로 전체 화면 후처리 패스 생성
        PostProcessFullScreenRenderPass(const std::string name) NOEXCEPTRELEASE;

        // 전체 화면 쿼드 렌더링 실행
        void Execute() NOEXCEPTRELEASE override;
    };
}