#pragma once

#include "Core/DxGraphicResource.h"
#include "Core/Exception/GraphicsException.h"

#include <memory>
#include <string>

// 전방 선언: 순환 참조 방지
class Drawable;
class TechniqueBase;
class DxGraphic;

namespace Graphic
{
    // DirectX 렌더링 파이프라인의 기본 인터페이스 클래스 (렌더링 파이프라인 설정, 객체 초기화, 기법 적용 등의 핵심 기능을 정의)
    class Render : public DxGraphicResource
    {
    public:
        // 렌더링 파이프라인을 설정하는 순수 가상 함수
        virtual void SetRenderPipeline() NOEXCEPTRELEASE = 0;
        
        /**
         * 부모 Drawable 객체에 대한 참조를 초기화
         * 
         * 렌더 객체가 특정 Drawable 객체와 연결될 때 호출되며,
         * 필요에 따라 부모 객체의 정보를 저장하거나 초기화 작업을 수행합니다.
         * 
         * @param drawable 부모 Drawable 객체의 상수 참조
         */
        virtual void InitializeParentReference(const Drawable&) noexcept { }
        
        /**
         * 기법(Technique) 객체를 받아들이는 Visitor 패턴 메서드
         * 
         * 다양한 렌더링 기법들이 이 렌더 객체에 접근하여
         * 특정 작업을 수행할 수 있도록 하는 인터페이스입니다.
         * 
         * @param technique 적용할 렌더링 기법 객체의 참조
         * 
         * @note Visitor 패턴을 통해 렌더 객체와 기법 객체 간의 결합도 감소
         */
        virtual void Accept(TechniqueBase&) { }

        virtual ~Render() = default;

        // 렌더 객체의 고유 식별자를 반환 (RenderManager에서 객체를 캐싱하고 관리하기 위해 사용되는 고유한 문자열 식별자를 반환)
        virtual std::string GetID() const noexcept
        {
            assert(false);
            return "";
        }
    };

    // 인스턴스 생성이 가능한 렌더 객체의 추상 클래스 (자기 자신의 복사본(인스턴스)을 생성하여 동일한 설정의 렌더 객체를 효율적으로 복제)
    class RenderInstance : public Render
    {
    public:
        virtual std::unique_ptr<RenderInstance> Instance() const noexcept = 0;
    };
}