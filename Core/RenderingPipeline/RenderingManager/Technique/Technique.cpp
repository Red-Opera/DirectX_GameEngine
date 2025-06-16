#include "stdafx.h"
#include "Technique.h"
#include "TechniqueBase.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/Exception/GraphicsException.h"

#include <sstream>

// 이름, 채널, 활성화 상태를 지정하는 생성자
Technique::Technique(std::string name, size_t channel, bool startActive) noexcept 
    : name(std::move(name)), channel(channel), isActive(startActive)
{

}

// 채널만 지정하는 생성자
Technique::Technique(size_t channel) : channel{ channel }
{

}

// 지정된 채널 필터로 렌더링 제출
void Technique::Submit(const Drawable& drawable, size_t channelFilter) const noexcept
{
    // 기법이 활성화되어 있고 채널 필터와 일치하는 경우에만 실행
    if (isActive && ((channel & channelFilter) != 0))
    {
        // 모든 렌더 스텝에 대해 렌더링 제출
        for (const auto& renderStep : renderSteps)
            renderStep.Submit(drawable);
    }
}

// 렌더 스텝 추가
void Technique::push_back(RenderStep renderStep) noexcept
{
    renderSteps.push_back(std::move(renderStep));
}

// 기법 활성화/비활성화 설정
void Technique::SetActive(bool active)
{
    isActive = active;
}

// 기법 활성화 상태 반환
bool Technique::GetAcive() const noexcept
{
    return isActive;
}

// 기법 이름 반환
const std::string Technique::GetName() const noexcept
{
    return name;
}

// 부모 Drawable 객체에 대한 참조 초기화
void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
    // 모든 렌더 스텝에 부모 참조 초기화
    for (auto& renderStep : renderSteps)
        renderStep.InitializeParentReference(parent);
}

// 렌더 그래프와 연결
void Technique::Link(RenderGraphNameSpace::RenderGraph& renderGraph)
{
    // 모든 렌더 스텝을 렌더 그래프에 연결
    for (auto& step : renderSteps)
        step.Link(renderGraph);
}

// Visitor 패턴으로 기법 방문 허용
void Technique::Accept(TechniqueBase& tech)
{
    // 방문자에게 현재 기법 설정
    tech.SetTechnique(this);

    // 모든 렌더 스텝이 방문자를 받아들이도록 함
    for (auto& step : renderSteps)
        step.Accept(tech);
}