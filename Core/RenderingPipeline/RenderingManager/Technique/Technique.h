#pragma once

#include "RenderStep.h"

#include <vector>
#include <string>

namespace RenderGraphNameSpace { class RenderGraph; }

class TechniqueBase;
class Drawable;

// 렌더링 기법을 정의하는 클래스 (여러 렌더 스텝들의 조합)
class Technique
{
public:
    Technique(size_t channel);                                                      // 채널만 지정하는 생성자
    Technique(std::string name, size_t channel, bool startActive = true) noexcept;  // 이름, 채널, 활성화 상태를 지정하는 생성자

    // 지정된 채널 필터로 렌더링 제출
    void Submit(const Drawable& drawable, size_t channelFilter) const noexcept;

    // 렌더 스텝 추가
    void push_back(RenderStep renderStep) noexcept;

    void SetActive(bool active);                // 기법 활성화/비활성화 설정
    bool GetAcive() const noexcept;             // 기법 활성화 상태 반환
    const std::string GetName() const noexcept; // 기법 이름 반환

    void InitializeParentReferences(const Drawable& parent) noexcept;   // 부모 Drawable 객체에 대한 참조 초기화
    void Link(RenderGraphNameSpace::RenderGraph&);                      // 렌더 그래프와 연결

    // Visitor 패턴으로 기법 방문 허용
    void Accept(TechniqueBase& tech);

private:
    std::vector<RenderStep> renderSteps;    // 이 기법을 구성하는 렌더 스텝들
    std::string name;                       // 기법의 이름

    size_t channel;                         // 렌더링 채널 (비트마스크)
    bool isActive = true;                   // 기법 활성화 상태
};