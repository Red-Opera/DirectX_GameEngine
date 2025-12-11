#pragma once

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/Render.h"

#include <vector>
#include <memory>
#include <string>

namespace RenderGraphNameSpace
{
    class RenderQueuePass;
    class RenderGraph;
}

class TechniqueBase;
class Drawable;

// 렌더링 기법을 구성하는 개별 렌더링 단계 클래스
class RenderStep
{
public:
    RenderStep(std::string targetPassName);             // 타겟 패스 이름으로 렌더 스텝 생성
    RenderStep(RenderStep&&) = default;                 // 이동 생성자 (기본 구현 사용)
    RenderStep(const RenderStep& renderStep) noexcept;  // 복사 생성자 (렌더 객체들을 복제하여 독립적인 인스턴스 생성)

    // 대입 연산자들 삭제 (안전한 리소스 관리를 위해)
    RenderStep& operator=(const RenderStep&) = delete;
    RenderStep& operator=(RenderStep&&) = delete;

    // 렌더 객체를 이 스텝에 추가
    void AddRender(std::shared_ptr<Graphic::Render> render) noexcept;

    void Submit(const Drawable& drawable) const;    // 지정된 Drawable 객체로 렌더링 작업을 렌더 큐에 제출
    void Accept(TechniqueBase& tech);               // Visitor 패턴으로 기법 방문 허용
    
    // 렌더 그래프와 연결하여 타겟 패스 찾기
    void Link(RenderGraphNameSpace::RenderGraph& renderGraph);

    // 모든 렌더 객체의 렌더링 파이프라인 설정
    void SetRenderPipeline() const NOEXCEPTRELEASE;

    // 부모 Drawable 객체에 대한 참조 초기화
    void InitializeParentReference(const class Drawable& parent) noexcept;

private:
    std::vector<std::shared_ptr<Graphic::Render>> renders;              // 이 스텝에 포함된 렌더 객체들
    RenderGraphNameSpace::RenderQueuePass* renderQueuePass = nullptr;   // 연결된 렌더 큐 패스
    
    std::string targetPassName; // 타겟 렌더 패스 이름
};