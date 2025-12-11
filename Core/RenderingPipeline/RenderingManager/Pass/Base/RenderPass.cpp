#include "stdafx.h"
#include "RenderPass.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Utility/StringConverter.h"

#include <sstream>
#include <algorithm>

namespace RenderGraphNameSpace
{
    // 패스 이름으로 생성자 초기화
    RenderPass::RenderPass(std::string name) noexcept : name(std::move(name))
    {

    }

    // 패스 리셋 (기본 구현은 빈 함수)
    void RenderPass::Reset() NOEXCEPTRELEASE
    {

    }

    // 모든 데이터 소비자 반환
    const std::vector<std::unique_ptr<PipelineDataConsumer>>& RenderPass::GetDataConsumers() const
    {
        return dataConsumers;
    }

    // 등록된 이름으로 데이터 제공자 검색
    PipelineDataProvider& RenderPass::GetDataProvider(const std::string& registeredName) const
    {
        // 모든 데이터 제공자에서 이름 검색
        for (auto& provider : dataProviders)
        {
            if (provider->GetName() == registeredName)
                return *provider;
        }

        // 찾지 못한 경우 상세한 예외 발생
        std::ostringstream oss;
        oss << "렌더 그래프 오류 : 렌더 패스 [" << GetName() << "]에서 데이터 제공자 [" << registeredName << "]를 찾을 수 없습니다.\n"
            << "사용 가능한 데이터 제공자 목록:\n";
        
        if (dataProviders.empty())
            oss << "  - (등록된 데이터 제공자가 없습니다)\n";
        
        else
        {
            for (const auto& provider : dataProviders)
                oss << "  - " << provider->GetName() << "\n";
        }
        
        oss << "올바른 데이터 제공자 이름을 사용하거나 해당 제공자를 먼저 등록해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 등록된 이름으로 데이터 소비자 검색
    PipelineDataConsumer& RenderPass::GetDataConsumer(const std::string& registeredName) const
    {
        // 모든 데이터 소비자에서 이름 검색
        for (auto& consumer : dataConsumers)
        {
            if (consumer->GetRegisteredName() == registeredName)
                return *consumer;
        }

        // 찾지 못한 경우 상세한 예외 발생
        std::ostringstream oss;
        oss << "렌더 그래프 오류: 렌더 패스 [" << GetName() << "]에서 데이터 소비자 [" << registeredName << "]를 찾을 수 없습니다.\n"
            << "사용 가능한 데이터 소비자 목록:\n";
        
        if (dataConsumers.empty())
            oss << "  - (등록된 데이터 소비자가 없습니다)\n";

        else
        {
            for (const auto& consumer : dataConsumers)
                oss << "  - " << consumer->GetRegisteredName() << "\n";
        }
        
        oss << "올바른 데이터 소비자 이름을 사용하거나 해당 소비자를 먼저 등록해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 데이터 소비자의 연결 대상 설정 ("패스이름.출력이름" 형식)
    void RenderPass::SetConsumerLinkage(const std::string& registeredName, const std::string& target)
    {
        // 데이터 소비자 검색
        auto& consumer = GetDataConsumer(registeredName);
        
        // 타겟 문자열을 "패스이름.출력이름" 형식으로 분리
        auto targetSplit = StringConverter::SplitString(target, ".");

        // 형식 검증 (반드시 "패스이름.출력이름" 형태여야 함)
        if (targetSplit.size() != 2u)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 연결 오류: 데이터 소비자 [" << registeredName << "]의 연결 대상 [" << target 
                << "]이 올바른 형식이 아닙니다.\n"
                << "올바른 형식: \"패스이름.출력이름\" (예: \"ShadowMapPass.shadowTexture\")\n"
                << "  - 패스 이름과 출력 이름을 점(.)으로 구분해야 합니다\n"
                << "  - 글로벌 출력의 경우: \"$.출력이름\" 형식을 사용하세요";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 빈 패스 이름이나 출력 이름 검증
        if (targetSplit[0].empty() || targetSplit[1].empty())
        {
            std::ostringstream oss;
            oss << "렌더 그래프 연결 오류: 데이터 소비자 [" << registeredName << "]의 연결 대상 [" << target 
                << "]에서 패스 이름 또는 출력 이름이 비어있습니다.\n"
                << "패스 이름: \"" << targetSplit[0] << "\"\n"
                << "출력 이름: \"" << targetSplit[1] << "\"\n"
                << "모든 부분이 유효한 이름을 가져야 합니다.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 데이터 소비자에 타겟 설정
        consumer.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
    }

    // 패스 이름 반환
    const std::string& RenderPass::GetName() const noexcept
    {
        return name;
    }

    // 패스 최종화 (모든 연결 상태 검증)
    void RenderPass::Finalize()
    {
        std::vector<std::string> consumerErrors;
        std::vector<std::string> providerErrors;

        // 모든 데이터 소비자의 연결 상태 검증
        for (auto& consumer : dataConsumers)
        {
            try
            {
                consumer->CheckLinkage();
            }

            catch (const std::exception& e)
            {
                consumerErrors.push_back(std::string("  - ") + e.what());
            }
        }

        // 모든 데이터 제공자의 연결 상태 검증
        for (auto& provider : dataProviders)
        {
            try
            {
                provider->CheckLinkage();
            }

            catch (const std::exception& e)
            {
                providerErrors.push_back(std::string("  - ") + e.what());
            }
        }

        // 오류가 있는 경우 통합 예외 발생
        if (!consumerErrors.empty() || !providerErrors.empty())
        {
            std::ostringstream oss;
            oss << "렌더 그래프 최종화 오류: 렌더 패스 [" << GetName() << "]에서 연결 오류가 발견되었습니다.\n";
            
            if (!consumerErrors.empty())
            {
                oss << "\n데이터 소비자 연결 오류:\n";

                for (const auto& error : consumerErrors)
                    oss << error << "\n";
            }
            
            if (!providerErrors.empty())
            {
                oss << "\n데이터 제공자 연결 오류:\n";

                for (const auto& error : providerErrors)
                    oss << error << "\n";
            }
            
            oss << "\n모든 연결 오류를 해결한 후 다시 시도해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    RenderPass::~RenderPass()
    {

    }

    // 데이터 소비자 추가 (중복 이름 검증 포함)
    void RenderPass::AddDataConsumer(std::unique_ptr<PipelineDataConsumer> newConsumer)
    {
        const std::string& newName = newConsumer->GetRegisteredName();

        // 중복 이름 검증
        for (const auto& existingConsumer : dataConsumers)
        {
            if (existingConsumer->GetRegisteredName() == newName)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 등록 오류: 렌더 패스 [" << GetName() << "]에 데이터 소비자 [" << newName 
                    << "]를 추가하려고 했지만, 같은 이름의 소비자가 이미 등록되어 있습니다.\n"
                    << "각 패스 내에서 데이터 소비자의 이름은 고유해야 합니다. 다른 이름을 사용하거나 기존 소비자를 제거해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        dataConsumers.push_back(std::move(newConsumer));
    }

    // 데이터 제공자 추가 (중복 이름 검증 포함)
    void RenderPass::AddDataProvider(std::unique_ptr<PipelineDataProvider> newProvider)
    {
        const std::string& newName = newProvider->GetName();

        // 중복 이름 검증
        for (const auto& existingProvider : dataProviders)
        {
            if (existingProvider->GetName() == newName)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 등록 오류: 렌더 패스 [" << GetName() << "]에 데이터 제공자 [" << newName 
                    << "]를 추가하려고 했지만, 같은 이름의 제공자가 이미 등록되어 있습니다.\n"
                    << "각 패스 내에서 데이터 제공자의 이름은 고유해야 합니다. 다른 이름을 사용하거나 기존 제공자를 제거해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        dataProviders.push_back(std::move(newProvider));
    }
}