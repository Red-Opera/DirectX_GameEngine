#pragma once

#include <string>
#include <memory>

#include "Core/Exception/RenderGraphCompileException.h"

// 전방 선언
namespace Graphic
{
    class Render;
    class BufferResource;
}

namespace RenderGraphNameSpace
{
    // 렌더 그래프에서 데이터를 제공하는 추상 기본 클래스
    class PipelineDataProvider
    {
    public:
        const std::string& GetName() const noexcept;        // 데이터 제공자의 이름 반환

        virtual void CheckLinkage() const = 0;                      // 연결 상태를 검증하는 순수 가상 함수
        virtual std::shared_ptr<Graphic::Render> GetRender();       // 렌더 객체를 반환 (기본적으로 예외 발생)
        virtual std::shared_ptr<Graphic::BufferResource> GetData(); // 버퍼 리소스를 반환 (기본적으로 예외 발생)

        virtual ~PipelineDataProvider() = default;

    protected:
        PipelineDataProvider(std::string name);             // 이름 검증을 포함한 생성자

    private:
        std::string name; // 데이터 제공자의 고유 이름
    };

    // 버퍼 리소스를 직접 제공하는 템플릿 클래스
    template<class T>
    class DirectBufferPipelineDataProvider : public PipelineDataProvider
    {
    public:
        // 정적 팩토리 메서드로 인스턴스 생성
        static std::unique_ptr<DirectBufferPipelineDataProvider> Create(std::string name, std::shared_ptr<T>& buffer)
        {
            return std::make_unique<DirectBufferPipelineDataProvider>(std::move(name), buffer);
        }

        // 이름과 버퍼 참조로 생성자 초기화
        DirectBufferPipelineDataProvider(std::string name, std::shared_ptr<T>& buffer)
            : PipelineDataProvider(std::move(name)), buffer(buffer)
        {

        }

        // 연결 상태 검증
        void CheckLinkage() const
        {

        }

        // 버퍼 데이터 반환 (중복 바인딩 방지)
        std::shared_ptr<Graphic::BufferResource> GetData() override
        {
            if (linked)
                throw RENDER_GRAPHIC_EXCEPTION("렌더 그래프 오류: 출력 [" + GetName() + "]이 이미 다른 입력에 연결되어 있습니다. 하나의 출력은 한 번만 사용할 수 있습니다.");

            linked = true;
            return buffer;
        }

    private:
        std::shared_ptr<T>& buffer; // 제공할 버퍼의 참조
        mutable bool linked = false; // 연결 상태 플래그 (const 함수에서 수정 가능하도록 mutable)
    };

    // 렌더링 파이프라인 객체를 직접 제공하는 템플릿 클래스
    template<class T>
    class DirectRenderPipelineDataProvider : public PipelineDataProvider
    {
    public:
        // 정적 팩토리 메서드로 인스턴스 생성
        static std::unique_ptr<DirectRenderPipelineDataProvider> Create(std::string name, std::shared_ptr<T>& buffer)
        {
            return std::make_unique<DirectRenderPipelineDataProvider>(std::move(name), buffer);
        }

        // 이름과 렌더 객체 참조로 생성자 초기화
        DirectRenderPipelineDataProvider(std::string name, std::shared_ptr<T>& render)
            : PipelineDataProvider(std::move(name)), render(render)
        {

        }

        // 연결 상태 검증
        void CheckLinkage() const
        {

        }

        // 렌더 객체 반환
        std::shared_ptr<Graphic::Render> GetRender() override
        {
            return render;
        }

    private:
        std::shared_ptr<T>& render; // 제공할 렌더 객체의 참조
    };
}