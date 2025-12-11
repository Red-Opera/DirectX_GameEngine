#pragma once

#include "Core/Camera/CameraFrustum.h"
#include "Core/Exception/WindowException.h"

#include <string>
#include <vector>
#include <memory>

class DxGraphic;
class Camera;

namespace Graphic
{
    class RenderTarget;
    class DepthStencil;
}

namespace RenderGraphNameSpace
{
    class PipelineDataProvider;
    class PipelineDataConsumer;
    class RenderPass;
    class RenderQueuePass;

    // 렌더링 파이프라인을 그래프 형태로 관리하는 메인 클래스
    class RenderGraph
    {
    public:
        // 백 버퍼와 마스터 깊이 버퍼로 렌더 그래프 초기화
        RenderGraph();
        ~RenderGraph();

        // 모든 렌더 패스를 순서대로 실행
        void Execute() NOEXCEPTRELEASE;
        // 모든 렌더 패스를 리셋 (다음 프레임 준비)
        void Reset() noexcept;

        // 지정된 이름의 렌더 큐 패스 반환
        RenderQueuePass& GetRenderQueue(const std::string& passName);

    protected:
        // 글로벌 소비자의 연결 대상 설정
        void SetGlobalConsumerTarget(const std::string& sinkName, const std::string& target);

        void AddRenderPass(std::unique_ptr<RenderPass> renderPass);     // 렌더 패스를 그래프에 추가
        void AddGlobalProvider(std::unique_ptr<PipelineDataProvider>);  // 글로벌 데이터 제공자 추가
        void AddGlobalConsumer(std::unique_ptr<PipelineDataConsumer>);  // 글로벌 데이터 소비자 추가

        // 이름으로 렌더 패스 찾기
        RenderPass& FindRenderPass(const std::string& name);

        // 렌더 그래프 최종화 (모든 연결 완료 후 실행 준비)
        void Finalize();

        std::shared_ptr<Graphic::RenderTarget> backBufferTarget; // 화면 출력용 백 버퍼
        std::shared_ptr<Graphic::DepthStencil> masterDepth;      // 기본 깊이 스텐실 버퍼

    private:
        void LinkDataConsumers(RenderPass& pass);   // 렌더 패스의 입력들을 해당 소비자들과 연결
        void LinkGlobalDataConsumers();             // 글로벌 소비자들을 해당 소비자들과 연결

        std::vector<std::unique_ptr<RenderPass>> renderPasses;                  // 렌더 패스 목록
        std::vector<std::unique_ptr<PipelineDataProvider>> globalDataProviders; // 글로벌 데이터 제공자들
        std::vector<std::unique_ptr<PipelineDataConsumer>> globalDataConsumers; // 글로벌 데이터 소비자들

        bool isFinalized = false; // 최종화 완료 여부
    };
}