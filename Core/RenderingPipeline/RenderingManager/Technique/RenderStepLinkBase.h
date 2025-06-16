#pragma once

#include "TechniqueBase.h"

namespace RenderGraphNameSpace { class RenderGraph; }

// 렌더 스텝을 렌더 그래프에 자동으로 연결하는 Visitor 클래스
class RenderStepLinkBase : public TechniqueBase
{
public:
    // 렌더 그래프 참조로 생성자 초기화
    explicit RenderStepLinkBase(RenderGraphNameSpace::RenderGraph& renderGraph)
        : renderGraph(renderGraph)
    {

    }

protected:
    // 렌더 스텝이 설정될 때 호출되는 콜백 (자동 연결 수행)
    void OnSetRenderStep() override;

private:
    RenderGraphNameSpace::RenderGraph& renderGraph; // 연결할 렌더 그래프의 참조
};