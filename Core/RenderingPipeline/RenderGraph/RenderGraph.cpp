#include "stdafx.h"
#include "RenderGraph.h"

#include "../Pipeline/OM/DepthStencil.h"
#include "../RenderGraph/PipelineDataConsumer.h"
#include "../RenderGraph/PipelineDataProvider.h"
#include "../RenderingPipeline.h"
#include "../RenderingManager/Pass/Base/RenderPass.h"
#include "../RenderingManager/Pass/Base/RenderQueuePass.h"
#include "../RenderTarget.h"

#include "Core/Camera/Camera.h"
#include "Core/Exception/RenderGraphCompileException.h"
#include "Utility/StringConverter.h"

#include <sstream>
#include <algorithm>

namespace RenderGraphNameSpace
{
    // 백 버퍼와 마스터 깊이 버퍼로 렌더 그래프 초기화
    RenderGraph::RenderGraph()
        : backBufferTarget(Window::GetDxGraphic().GetRenderTarget()),
        masterDepth(std::make_shared<Graphic::OutputOnlyDepthStencil>())
    {
        // 글로벌 제공자들 등록 (모든 패스에서 접근 가능한 기본 리소스들)
        AddGlobalProvider(DirectBufferPipelineDataProvider<Graphic::RenderTarget>::Create("backbuffer", backBufferTarget));
        AddGlobalProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("masterDepth", masterDepth));
        
        // 글로벌 소비자 등록 (최종 출력을 받을 소비자)
        AddGlobalConsumer(DirectBufferDataConsumer<Graphic::RenderTarget>::Create("backbuffer", backBufferTarget));
    }

    RenderGraph::~RenderGraph()
    {

    }

    // 글로벌 소비자의 연결 대상 설정
    void RenderGraph::SetGlobalConsumerTarget(const std::string& sinkName, const std::string& target)
    {
        // 지정된 이름의 글로벌 소비자 찾기
        const auto finder = [&sinkName](const std::unique_ptr<PipelineDataConsumer>& passInput) { 
            return passInput->GetRegisteredName() == sinkName; 
        };
        const auto i = std::find_if(globalDataConsumers.begin(), globalDataConsumers.end(), finder);

        if (i == globalDataConsumers.end())
        {
            std::ostringstream oss;
            oss << "렌더 그래프 연결 오류: 글로벌 소비자 [" << sinkName << "]를 찾을 수 없습니다.\n"
                << "등록된 글로벌 소비자 목록:\n";
            
            if (globalDataConsumers.empty())
                oss << "  - (등록된 글로벌 소비자가 없습니다)\n";

            else
            {
                for (const auto& consumer : globalDataConsumers)
                    oss << "  - " << consumer->GetRegisteredName() << "\n";
            }
            
            oss << "올바른 글로벌 소비자 이름을 사용하거나 해당 소비자를 먼저 등록해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 타겟 문자열을 "패스이름.출력이름" 형식으로 분리
        auto targetSplit = StringConverter::SplitString(target, ".");

        if (targetSplit.size() != 2u)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 연결 오류 : 글로벌 소비자 [" << sinkName << "]의 연결 대상 [" << target 
                << "]이 올바른 형식이 아닙니다.\n"
                << "올바른 형식 : \"패스이름.출력이름\" (예: \"GeometryPass.renderTarget\")\n"
                << "글로벌 출력의 경우 : \"$.출력이름\" 형식을 사용하세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 소비자에 타겟 설정
        (*i)->SetTarget(targetSplit[0], targetSplit[1]);
    }

    // 렌더 패스를 그래프에 추가
    void RenderGraph::AddRenderPass(std::unique_ptr<RenderPass> renderPass)
    {
        // 최종화 후에는 패스 추가 불가
        if (isFinalized)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 추가 오류: 이미 최종화된 렌더 그래프에는 패스를 추가할 수 없습니다.\n"
                << "추가하려는 패스: [" << renderPass->GetName() << "]\n"
                << "새로운 렌더 그래프를 생성하거나 최종화 이전에 모든 패스를 추가해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 중복 패스 이름 검증
        for (const auto& pass : renderPasses)
        {
            if (renderPass->GetName() == pass->GetName())
            {
                std::ostringstream oss;
                oss << "렌더 그래프 추가 오류: 패스 이름 [" << renderPass->GetName() << "]이 이미 존재합니다.\n"
                    << "렌더 그래프 내에서 패스 이름은 고유해야 합니다.\n"
                    << "다른 이름을 사용하거나 기존 패스를 제거해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        // 패스의 입력들을 기존 출력들과 연결
        LinkDataConsumers(*renderPass);

        // 패스를 목록에 추가
        renderPasses.push_back(std::move(renderPass));
    }

    // 글로벌 데이터 소비자 추가
    void RenderGraph::AddGlobalConsumer(std::unique_ptr<PipelineDataConsumer> input)
    {
        globalDataConsumers.push_back(std::move(input));
    }

    // 이름으로 렌더 패스 찾기
    RenderPass& RenderGraph::FindRenderPass(const std::string& name)
    {
        const auto i = std::find_if(renderPasses.begin(), renderPasses.end(), 
            [&name](auto& renderPass) { return renderPass->GetName() == name; });

        if (i == renderPasses.end())
        {
            std::ostringstream oss;
            oss << "렌더 그래프 검색 오류: 패스 [" << name << "]를 찾을 수 없습니다.\n"
                << "등록된 렌더 패스 목록:\n";
            
            if (renderPasses.empty())
                oss << "  - (등록된 렌더 패스가 없습니다)\n";

            else
            {
                for (const auto& pass : renderPasses)
                    oss << "  - " << pass->GetName() << "\n";
            }
            
            oss << "올바른 패스 이름을 사용하거나 해당 패스를 먼저 추가해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        return **i;
    }

    // 글로벌 데이터 제공자 추가
    void RenderGraph::AddGlobalProvider(std::unique_ptr<PipelineDataProvider> output)
    {
        globalDataProviders.push_back(std::move(output));
    }

    // 모든 렌더 패스를 순서대로 실행
    void RenderGraph::Execute() NOEXCEPTRELEASE
    {
        // 최종화되지 않은 그래프는 실행 불가
        if (!isFinalized)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 실행 오류: 최종화되지 않은 렌더 그래프는 실행할 수 없습니다.\n"
                << "Finalize() 메서드를 먼저 호출해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 등록된 순서대로 모든 패스 실행
        for (auto& pass : renderPasses)
            pass->Execute();
    }

    // 모든 렌더 패스를 리셋 (다음 프레임 준비)
    void RenderGraph::Reset() noexcept
    {
        // 최종화되지 않은 그래프는 리셋 불가 (하지만 noexcept이므로 assert 사용)
        assert(isFinalized);

        // 모든 패스를 리셋하여 다음 프레임 준비
        for (auto& pass : renderPasses)
            pass->Reset();
    }

    // 렌더 그래프 최종화 (모든 연결 완료 후 실행 준비)
    void RenderGraph::Finalize()
    {
        // 이미 최종화된 경우 중복 방지
        if (isFinalized)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 최종화 오류: 이미 최종화된 렌더 그래프입니다.\n"
                << "중복 최종화는 허용되지 않습니다.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 모든 패스 최종화 (연결 상태 검증)
        for (const auto& pass : renderPasses)
            pass->Finalize();

        // 글로벌 소비자들 연결
        LinkGlobalDataConsumers();
        
        // 최종화 완료 표시
        isFinalized = true;
    }

    // 지정된 이름의 렌더 큐 패스 반환
    RenderQueuePass& RenderGraph::GetRenderQueue(const std::string& passName)
    {
        // 지정된 이름의 패스 찾기
        for (const auto& pass : renderPasses)
        {
            if (pass->GetName() == passName)
            {
                // RenderQueuePass로 캐스팅 시도
                try
                {
                    return dynamic_cast<RenderQueuePass&>(*pass);
                }

                catch (std::bad_cast&)
                {
                    std::ostringstream oss;
                    oss << "렌더 그래프 타입 오류: 패스 [" << passName << "]는 RenderQueuePass가 아닙니다.\n"
                        << "해당 패스는 다른 타입의 렌더 패스입니다.\n"
                        << "RenderQueuePass 타입의 패스만 GetRenderQueue()로 접근할 수 있습니다.";

                    throw RENDER_GRAPHIC_EXCEPTION(oss.str());
                }
            }
        }

        // 패스를 찾지 못한 경우
        std::ostringstream oss;
        oss << "렌더 그래프 검색 오류: RenderQueuePass [" << passName << "]를 찾을 수 없습니다.\n"
            << "등록된 렌더 패스 목록:\n";
        
        if (renderPasses.empty())
            oss << "  - (등록된 렌더 패스가 없습니다)\n";

        else
        {
            for (const auto& pass : renderPasses)
            {
                oss << "  - " << pass->GetName();
                // 타입 정보도 함께 표시
                if (dynamic_cast<RenderQueuePass*>(pass.get()))
                    oss << " (RenderQueuePass)";
                else
                    oss << " (기타 RenderPass)";
                oss << "\n";
            }
        }
        
        oss << "올바른 RenderQueuePass 이름을 사용하거나 해당 패스를 먼저 추가해주세요.";

        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 렌더 패스의 입력들을 해당 출력들과 연결
    void RenderGraph::LinkDataConsumers(RenderPass& pass)
    {
        // 패스의 모든 입력(소비자)에 대해 연결 수행
        for (auto& passInput : pass.GetDataConsumers())
        {
            const auto& inputSourcePassName = passInput->GetPassName();

            // 입력 소스 패스가 설정되지 않은 경우
            if (inputSourcePassName.empty())
            {
                std::ostringstream oss;
                oss << "렌더 그래프 연결 오류: 패스 [" << pass.GetName() << "]의 입력 [" 
                    << passInput->GetRegisteredName() << "]에 소스 패스가 설정되지 않았습니다.\n"
                    << "SetTarget() 메서드를 사용하여 연결 대상을 설정해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 글로벌 출력에 연결하는 경우 ("$" 패스 이름)
            if (inputSourcePassName == "$")
            {
                bool isRenderSet = false;

                // 글로벌 제공자에서 해당 출력 찾기
                for (auto& source : globalDataProviders)
                {
                    if (source->GetName() == passInput->GetOutputName())
                    {
                        passInput->SetConsumeData(*source);
                        isRenderSet = true;
                        break;
                    }
                }

                if (!isRenderSet)
                {
                    std::ostringstream oss;
                    oss << "렌더 그래프 연결 오류: 글로벌 출력 [" << passInput->GetOutputName() 
                        << "]를 찾을 수 없습니다.\n"
                        << "등록된 글로벌 제공자 목록:\n";
                    
                    if (globalDataProviders.empty())
                        oss << "  - (등록된 글로벌 제공자가 없습니다)\n";

                    else
                    {
                        for (const auto& provider : globalDataProviders)
                            oss << "  - " << provider->GetName() << "\n";
                    }
                    
                    oss << "올바른 글로벌 출력 이름을 사용하거나 해당 제공자를 먼저 등록해주세요.";

                    throw RENDER_GRAPHIC_EXCEPTION(oss.str());
                }
            }

            // 특정 패스의 출력에 연결하는 경우
            else
            {
                bool bound = false;
                
                // 해당 이름의 패스 찾기
                for (auto& sourcePass : renderPasses)
                {
                    if (sourcePass->GetName() == inputSourcePassName)
                    {
                        try
                        {
                            auto& source = sourcePass->GetDataProvider(passInput->GetOutputName());
                            passInput->SetConsumeData(source);
                            bound = true;
                            break;
                        }

                        catch (const std::exception& e)
                        {
                            std::ostringstream oss;
                            oss << "렌더 그래프 연결 오류: 패스 [" << inputSourcePassName 
                                << "]에서 출력 [" << passInput->GetOutputName() << "]를 찾을 수 없습니다.\n"
                                << "오류 내용: " << e.what();

                            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
                        }
                    }
                }

                if (!bound)
                {
                    std::ostringstream oss;
                    oss << "렌더 그래프 연결 오류: 소스 패스 [" << inputSourcePassName << "]를 찾을 수 없습니다.\n"
                        << "등록된 렌더 패스 목록:\n";
                    
                    if (renderPasses.empty())
                        oss << "  - (등록된 렌더 패스가 없습니다)\n";

                    else
                    {
                        for (const auto& existingPass : renderPasses)
                            oss << "  - " << existingPass->GetName() << "\n";
                    }
                    
                    oss << "올바른 패스 이름을 사용하거나 해당 패스를 먼저 추가해주세요.";

                    throw RENDER_GRAPHIC_EXCEPTION(oss.str());
                }
            }
        }
    }

    // 글로벌 소비자들을 해당 출력들과 연결
    void RenderGraph::LinkGlobalDataConsumers()
    {
        for (auto& consumer : globalDataConsumers)
        {
            const auto& inputSourcePassName = consumer->GetPassName();

            // 글로벌 소비자가 연결 대상이 설정되지 않은 경우 건너뛰기
            if (inputSourcePassName.empty())
                continue;

            bool found = false;

            // 해당 패스에서 출력 찾기
            for (auto& existingPass : renderPasses)
            {
                if (existingPass->GetName() == inputSourcePassName)
                {
                    try
                    {
                        auto& source = existingPass->GetDataProvider(consumer->GetOutputName());
                        consumer->SetConsumeData(source);
                        found = true;
                        break;
                    }

                    catch (const std::exception& e)
                    {
                        std::ostringstream oss;
                        oss << "렌더 그래프 글로벌 연결 오류 : 패스 [" << inputSourcePassName 
                            << "]에서 출력 [" << consumer->GetOutputName() << "]를 찾을 수 없습니다.\n"
                            << "글로벌 소비자 : [" << consumer->GetRegisteredName() << "]\n"
                            << "오류 내용 : " << e.what();

                        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
                    }
                }
            }

            if (!found)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 글로벌 연결 오류: 글로벌 소비자 [" << consumer->GetRegisteredName() 
                    << "]의 소스 패스 [" << inputSourcePassName << "]를 찾을 수 없습니다.\n"
                    << "등록된 렌더 패스 목록:\n";
                
                for (const auto& existingPass : renderPasses)
                    oss << "  - " << existingPass->GetName() << "\n";
                
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }
    }
}
