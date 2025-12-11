#pragma once

#include "RenderPass.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"

namespace Graphic { class Render; }

namespace RenderGraphNameSpace
{
    // 실제 렌더링 작업을 수행하는 렌더 패스의 기본 클래스
    class RenderingPass : public RenderPass
    {
    protected:
        // 패스 이름과 초기 렌더 객체 목록으로 생성자 초기화
        RenderingPass(std::string name, std::vector<std::shared_ptr<Graphic::Render>> renders = { });

        void AddRender(std::shared_ptr<Graphic::Render> render) noexcept;   // 렌더 객체를 패스에 추가
        void RenderAll() const NOEXCEPTRELEASE;                             // 모든 렌더 객체의 렌더링 파이프라인을 실행

        // 패스 최종화 (렌더 타겟 또는 깊이 스텐실 존재 검증)
        void Finalize() override;

        // 템플릿 기반 렌더 객체 입력 슬롯 추가
        template<class T>
        void AddRenderDataConsumer(std::string name)
        {
            // 새 렌더 객체를 위한 인덱스 계산
            const auto index = renders.size();
            
            // 빈 슬롯을 렌더 컨테이너에 추가
            renders.emplace_back();

            // 해당 인덱스에 렌더 객체를 받을 데이터 소비자 추가
            AddDataConsumer(std::make_unique<ContainRenderPipelineDataConsumer<T>>(std::move(name), renders, index));
        }

        std::shared_ptr<Graphic::RenderTarget> renderTarget; // 렌더 타겟 (색상 버퍼)
        std::shared_ptr<Graphic::DepthStencil> depthStencil; // 깊이 스텐실 버퍼

    private:
        // 렌더 타겟과 깊이 스텐실을 렌더링 파이프라인에 바인딩
        void RenderBufferResources() const NOEXCEPTRELEASE;

        std::vector<std::shared_ptr<Graphic::Render>> renders; // 실행할 렌더 객체 목록
    };
}