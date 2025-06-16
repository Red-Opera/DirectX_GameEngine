#pragma once

#include "Core/Exception/GraphicsException.h"

#include <array>
#include <string>
#include <vector>
#include <memory>

// 전방 선언
class DxGraphic;

namespace Graphic
{
    class RenderTarget;
    class DepthStencil;
}

namespace RenderGraphNameSpace
{
    class PipelineDataConsumer;
    class PipelineDataProvider;

    // 렌더 그래프의 기본 렌더 패스 추상 클래스
    class RenderPass
    {
    public:
        // 패스 이름으로 생성자 초기화
        RenderPass(std::string name) noexcept;

        virtual void Execute() NOEXCEPTRELEASE = 0;     // 패스 실행을 위한 순수 가상 함수 (파생 클래스에서 반드시 구현)
        virtual void Reset() NOEXCEPTRELEASE;           // 패스 리셋 (기본 구현은 빈 함수)

        // 모든 데이터 소비자 반환
        const std::vector<std::unique_ptr<PipelineDataConsumer>>& GetDataConsumers() const;

        PipelineDataProvider& GetDataProvider(const std::string& registeredName) const;     // 등록된 이름으로 데이터 제공자 검색
        PipelineDataConsumer& GetDataConsumer(const std::string& registeredName) const;     // 등록된 이름으로 데이터 소비자 검색

        // 데이터 소비자의 연결 대상 설정 ("패스이름.출력이름" 형식)
        void SetConsumerLinkage(const std::string& registeredName, const std::string& target);

        // 패스 이름 반환
        const std::string& GetName() const noexcept;

        // 패스 최종화 (모든 연결 상태 검증)
        virtual void Finalize();
        virtual ~RenderPass();

    protected:
        void AddDataConsumer(std::unique_ptr<PipelineDataConsumer> newConsumer);    // 데이터 소비자 추가 (중복 이름 검증 포함)
        void AddDataProvider(std::unique_ptr<PipelineDataProvider> newProvider);    // 데이터 제공자 추가 (중복 이름 검증 포함)

    private:
        std::vector<std::unique_ptr<PipelineDataConsumer>> dataConsumers; // 데이터 소비자 목록
        std::vector<std::unique_ptr<PipelineDataProvider>> dataProviders; // 데이터 제공자 목록

        std::string name; // 패스의 고유 이름
    };
}