#pragma once
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"

#include "Utility/MathInfo.h"

#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

// 전방 선언
namespace Graphic
{
    class Render;
}

namespace RenderGraphNameSpace
{
    class RenderPass;

    // 렌더 그래프에서 데이터를 소비하는 추상 기본 클래스
    class PipelineDataConsumer
    {
    public:
        const std::string& GetRegisteredName() const noexcept;  // 등록된 이름 반환
        const std::string& GetPassName() const noexcept;        // 연결된 패스 이름 반환
        const std::string& GetOutputName() const noexcept;      // 연결된 출력 이름 반환

        // 타겟 패스와 출력 이름 설정
        void SetTarget(std::string passName, std::string outputName);

        virtual void SetConsumeData(PipelineDataProvider& source) = 0;  // 데이터 제공자로부터 데이터 설정하는 순수 가상 함수
        virtual void CheckLinkage() const = 0;                          // 연결 상태 검증하는 순수 가상 함수

        virtual ~PipelineDataConsumer() = default;

    protected:
        PipelineDataConsumer(std::string registeredName);

    private:
        std::string registeredName; // 소비자의 등록된 이름
        std::string passName;       // 연결된 패스 이름
        std::string outputName;     // 연결된 출력 이름
    };

    // 버퍼 리소스를 직접 소비하는 템플릿 클래스
    template<class T>
    class DirectBufferDataConsumer : public PipelineDataConsumer
    {
        // T가 BufferResource를 상속받는지 컴파일 타임 검증
        static_assert(std::is_base_of_v<Graphic::BufferResource, T>, "DirectBufferDataConsumer 타입 오류 : 템플릿 매개변수는 BufferResource를 상속받는 타입이어야 합니다");

    public:
        // 정적 팩토리 메서드로 인스턴스 생성
        static std::unique_ptr<PipelineDataConsumer> Create(std::string registeredName, std::shared_ptr<T>& target)
        {
            return std::make_unique<DirectBufferDataConsumer>(std::move(registeredName), target);
        }

        // 연결 상태 검증 (연결되지 않으면 예외 발생)
        void CheckLinkage() const override
        {
            if (!linked)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 연결 오류: 입력 [" << GetRegisteredName() << "]이 어떤 출력에도 연결되지 않았습니다. "
                    << "SetTarget() 메서드를 사용하여 유효한 출력에 연결해주세요.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        // 데이터 제공자로부터 버퍼 데이터 설정
        void SetConsumeData(PipelineDataProvider& source) override
        {
            // 타입 캐스팅 시도
            auto p = std::dynamic_pointer_cast<T>(source.GetData());

            if (!p)
            {
                // 타입 불일치 시 상세한 에러 메시지 생성
                std::ostringstream oss;
                oss << "렌더 그래프 타입 불일치 오류: 입력 [" << GetRegisteredName() << "]을 출력 [" 
                    << GetPassName() << "." << GetOutputName() << "]에 연결하려고 했지만 타입이 호환되지 않습니다.\n"
                    << "  - 요구되는 타입: " << typeid(T).name() << "\n"
                    << "  - 제공되는 타입: " << typeid(*source.GetData().get()).name() << "\n"
                    << "호환되는 타입의 출력에 연결하거나 올바른 입력 타입을 사용해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            target = std::move(p);
            linked = true;
        }

        // 등록 이름과 타겟 참조로 생성자 초기화
        DirectBufferDataConsumer(std::string registeredName, std::shared_ptr<T>& render)
            : PipelineDataConsumer(std::move(registeredName)), target(render)
        {

        }

    private:
        std::shared_ptr<T>& target; // 설정할 타겟 버퍼의 참조
        bool linked = false;        // 연결 상태 플래그
    };

    // 컨테이너 내 렌더 객체를 소비하는 템플릿 클래스
    template<class T>
    class ContainRenderPipelineDataConsumer : public PipelineDataConsumer
    {
        // T가 Render를 상속받는지 컴파일 타임 검증
        static_assert(std::is_base_of_v<Graphic::Render, T>, "ContainRenderPipelineDataConsumer 타입 오류: 템플릿 매개변수는 Render를 상속받는 타입이어야 합니다");

    public:
        // 연결 상태 검증 (연결되지 않으면 예외 발생)
        void CheckLinkage() const override
        {
            if (!linked)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 연결 오류: 컨테이너 입력 [" << GetRegisteredName() << "]이 어떤 출력에도 연결되지 않았습니다. "
                    << "SetTarget() 메서드를 사용하여 유효한 렌더 출력에 연결해주세요.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        // 데이터 제공자로부터 렌더 객체를 컨테이너에 설정
        void SetConsumeData(PipelineDataProvider& source) override
        {
            // 타입 캐스팅 시도
            auto p = std::dynamic_pointer_cast<T>(source.GetRender());

            if (!p)
            {
                // 타입 불일치 시 상세한 에러 메시지 생성
                std::ostringstream oss;
                oss << "렌더 그래프 타입 불일치 오류: 컨테이너 입력 [" << GetRegisteredName() << "]을 출력 [" 
                    << GetPassName() << "." << GetOutputName() << "]에 연결하려고 했지만 렌더 타입이 호환되지 않습니다.\n"
                    << "  - 요구되는 렌더 타입: " << typeid(T).name() << "\n"
                    << "  - 제공되는 렌더 타입: " << typeid(*source.GetRender().get()).name() << "\n"
                    << "호환되는 렌더 타입의 출력에 연결해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            container[index] = std::move(p);
            linked = true;
        }

        // 등록 이름, 컨테이너 참조, 인덱스로 생성자 초기화
        ContainRenderPipelineDataConsumer(std::string registeredName, std::vector<std::shared_ptr<Graphic::Render>>& container, size_t index)
            : PipelineDataConsumer(std::move(registeredName)), container(container), index(index)
        {

        }

    private:
        std::vector<std::shared_ptr<Graphic::Render>>& container;   // 렌더링 파이프라인 객체 타겟 컨테이너의 참조
        size_t index;                                               // 컨테이너 내 인덱스
        bool linked = false;                                        // 연결 상태 플래그
    };

    // 렌더 객체를 직접 소비하는 템플릿 클래스
    template<class T>
    class DirectRenderPipelineDataConsumer : public PipelineDataConsumer
    {
        // T가 Render를 상속받는지 컴파일 타임 검증
        static_assert(std::is_base_of_v<Graphic::Render, T>, "DirectRenderPipelineDataConsumer 타입 오류: 템플릿 매개변수는 Render를 상속받는 타입이어야 합니다");
    public:
        // 정적 팩토리 메서드로 인스턴스 생성
        static std::unique_ptr<PipelineDataConsumer> Create(std::string registeredName, std::shared_ptr<T>& target)
        {
            return std::make_unique<DirectRenderPipelineDataConsumer>(std::move(registeredName), target);
        }

        // 연결 상태 검증 (연결되지 않으면 예외 발생)
        void CheckLinkage() const override
        {
            if (!linked)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 연결 오류: 렌더 입력 [" << GetRegisteredName() << "]이 어떤 출력에도 연결되지 않았습니다. "
                    << "SetTarget() 메서드를 사용하여 유효한 렌더 출력에 연결해주세요.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }

        // 데이터 제공자로부터 렌더 객체 설정
        void SetConsumeData(PipelineDataProvider& source) override
        {
            // 타입 캐스팅 시도
            auto p = std::dynamic_pointer_cast<T>(source.GetRender());

            if (!p)
            {
                // 타입 불일치 시 상세한 에러 메시지 생성
                std::ostringstream oss;
                oss << "렌더 그래프 타입 불일치 오류: 렌더 입력 [" << GetRegisteredName() << "]을 출력 [" 
                    << GetPassName() << "." << GetOutputName() << "]에 연결하려고 했지만 렌더 타입이 호환되지 않습니다.\n"
                    << "  - 요구되는 렌더 타입: " << typeid(T).name() << "\n"
                    << "  - 제공되는 렌더 타입: " << typeid(*source.GetRender().get()).name() << "\n"
                    << "호환되는 렌더 타입의 출력에 연결하거나 올바른 입력 타입을 사용해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            target = std::move(p);
            linked = true;
        }

        // 등록 이름과 타겟 참조로 생성자 초기화
        DirectRenderPipelineDataConsumer(std::string registeredName, std::shared_ptr<T>& target)
            : PipelineDataConsumer(std::move(registeredName)), target(target)
        {

        }

    private:
        std::shared_ptr<T>& target; // 설정할 타겟 렌더 객체의 참조
        bool linked = false;        // 연결 상태 플래그
    };
}