#include "stdafx.h"
#include "RenderStep.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderQueuePass.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

#include <sstream>

// 타겟 패스 이름으로 렌더 스텝 생성
RenderStep::RenderStep(std::string targetPassName) : targetPassName(std::move(targetPassName))
{
    // 빈 타겟 패스 이름 검증
    if (this->targetPassName.empty())
    {
        std::ostringstream oss;
        oss << "렌더 스텝 생성 오류: 타겟 패스 이름이 비어있습니다.\n"
            << "유효한 렌더 패스 이름을 제공해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }
}

// 복사 생성자 (렌더 객체들을 복제하여 독립적인 인스턴스 생성)
RenderStep::RenderStep(const RenderStep& renderStep) noexcept : targetPassName(renderStep.targetPassName)
{
    // 효율적인 메모리 할당을 위해 벡터 크기 미리 예약
    renders.reserve(renderStep.renders.size());

    // 각 렌더 객체를 복사 (인스턴스 생성 가능한 객체는 새 인스턴스 생성)
    for (auto& render : renderStep.renders)
    {
        // RenderInstance인 경우 새 인스턴스 생성
        if (auto* instance = dynamic_cast<const Graphic::RenderInstance*>(render.get()))
            renders.push_back(instance->Instance());

        // 일반 Render 객체인 경우 공유 포인터 복사
        else
            renders.push_back(render);
    }
}

// 렌더 객체를 이 스텝에 추가
void RenderStep::AddRender(std::shared_ptr<Graphic::Render> render) noexcept
{
    renders.push_back(std::move(render));
}

// 지정된 Drawable 객체로 렌더링 작업을 렌더 큐에 제출
void RenderStep::Submit(const Drawable& drawable) const
{
    // 렌더 큐 패스가 연결되지 않은 경우 예외 발생
    if (renderQueuePass == nullptr)
    {
        std::ostringstream oss;
        oss << "렌더 스텝 제출 오류: 렌더 큐 패스가 연결되지 않았습니다.\n"
            << "타겟 패스 이름 : [" << targetPassName << "]\n"
            << "Link() 메서드를 먼저 호출하여 렌더 그래프와 연결해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 렌더 작업을 렌더 큐에 제출
    renderQueuePass->Accept(RenderGraphNameSpace::RenderJob{ this, &drawable });
}

// Visitor 패턴으로 기법 방문 허용
void RenderStep::Accept(TechniqueBase& tech)
{
    // 방문자에게 현재 렌더 스텝 설정
    tech.SetRenderStep(this);

    // 모든 렌더 객체가 방문자를 받아들이도록 함
    for (auto& render : renders)
        render->Accept(tech);
}

// 렌더 그래프와 연결하여 타겟 패스 찾기
void RenderStep::Link(RenderGraphNameSpace::RenderGraph& renderGraph)
{
    // 이미 연결된 경우 중복 연결 방지
    if (renderQueuePass != nullptr)
    {
        std::ostringstream oss;
        oss << "렌더 스텝 연결 오류 : 이미 렌더 큐 패스에 연결되어 있습니다.\n"
            << "현재 연결된 패스 : [" << targetPassName << "]\n"
            << "중복 연결은 허용되지 않습니다.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    try
    {
        // 렌더 그래프에서 타겟 패스 찾기
        renderQueuePass = &renderGraph.GetRenderQueue(targetPassName);
    }

    catch (const std::exception& e)
    {
        std::ostringstream oss;
        oss << "렌더 스텝 연결 오류: 타겟 렌더 큐 패스를 찾을 수 없습니다.\n"
            << "타겟 패스 이름: [" << targetPassName << "]\n"
            << "오류 내용: " << e.what() << "\n"
            << "렌더 그래프에 해당 패스가 등록되어 있는지 확인해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }
}

// 모든 렌더 객체의 렌더링 파이프라인 설정
void RenderStep::SetRenderPipeline() const NOEXCEPTRELEASE
{
    // 순서대로 모든 렌더 객체의 파이프라인 설정
    for (const auto& render : renders)
        render->SetRenderPipeline();
}

// 부모 Drawable 객체에 대한 참조 초기화
void RenderStep::InitializeParentReference(const Drawable& parent) noexcept
{
    // 모든 렌더 객체에 부모 참조 초기화
    for (auto& render : renders)
        render->InitializeParentReference(parent);
}
