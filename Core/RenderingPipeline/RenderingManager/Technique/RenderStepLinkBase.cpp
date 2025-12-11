#include "stdafx.h"
#include "RenderStepLinkBase.h"
#include "RenderStep.h"

#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Core/Exception/RenderGraphCompileException.h"

#include <sstream>

// 렌더 스텝이 설정될 때 호출되는 콜백 (자동 연결 수행)
void RenderStepLinkBase::OnSetRenderStep()
{
    // 렌더 스텝이 유효한지 확인
    if (renderStep == nullptr)
    {
        std::ostringstream oss;
        oss << "렌더 스텝 연결 오류: 연결하려는 렌더 스텝이 null입니다.\n"
            << "유효한 RenderStep 객체가 설정되어야 합니다.\n"
            << "TechniqueBase::SetRenderStep() 호출 시 유효한 포인터를 전달해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    try
    {
        // 렌더 스텝을 렌더 그래프에 자동 연결
        renderStep->Link(renderGraph);
    }

    catch (const std::exception& e)
    {
        std::ostringstream oss;
        oss << "렌더 스텝 자동 연결 실패: 렌더 스텝을 렌더 그래프에 연결하는 중 오류가 발생했습니다.\n"
            << "오류 내용: " << e.what() << "\n"
            << "렌더 그래프에 해당 타겟 패스가 등록되어 있는지 확인해주세요.\n"
            << "또는 렌더 스텝이 이미 다른 그래프에 연결되어 있을 수 있습니다.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());


    }
}