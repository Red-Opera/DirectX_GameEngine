#pragma once

#include "Render.h"

#include "../DxGraphic.h"

#include <memory>
#include <type_traits>
#include <unordered_map>

namespace Graphic
{
    // DirectX 렌더링 파이프라인의 렌더 객체들을 관리하는 싱글톤 매니저 클래스 (렌더 객체들의 생성, 저장, 재사용을 담당)
    class RenderManager
    {
    public:
        // 지정된 타입의 렌더링 파이프라인 객체를 가져오거나 생성하는 정적 메서드 (T : Render 클래스를 상속받은 요청할 렌더 객체의 타입)
        template<class T, typename... Params>
        static std::shared_ptr<T> GetRender(Params&&... path) NOEXCEPTRELEASE
        {
            // 템플릿 매개변수 T가 Render 클래스를 상속받는지 컴파일 타임 검증
            static_assert(std::is_base_of<Render, T>::value, "해당 클래스는 Rendering Pipeline의 대상이 아닙니다.");

            // 싱글톤 인스턴스를 통해 실제 구현 메서드 호출 (요청된 렌더 객체의 스마트 포인터)
            return Instance().GetRender_<T>(std::forward<Params>(path)...);
        }

    private:
        /**
         * 렌더 객체를 실제로 관리하는 내부 구현 메서드
         * 
         * 동작 과정:
         * 1. T::CreateID()를 호출하여 고유 키 생성
         * 2. 키가 이미 존재하는지 renders 맵에서 검색
         * 3. 존재하면 기존 객체 반환, 없으면 새 객체 생성 후 저장
         */
        template<class T, typename... Params>
        std::shared_ptr<T> GetRender_(Params&&... path) NOEXCEPTRELEASE
        {
            // 전달된 매개변수들을 사용하여 고유 키 생성
            const auto key = T::CreateID(std::forward<Params>(path)...);
            
            // renders 맵에서 키에 해당하는 객체 검색
            auto i = renders.find(key);	

            // 키가 존재하지 않으면 새 객체 생성
            if (i == renders.end())
            {
                // 새로운 렌더 객체 생성 (완벽 전달 사용)
                auto render = std::make_shared<T>(std::forward<Params>(path)...); 
                
                // 생성된 객체를 맵에 저장
                renders[key] = render;

                return render;
            }

            // 키가 존재하면 기존 객체를 적절한 타입으로 캐스팅하여 반환
            else
                return std::static_pointer_cast<T>(i->second);
        }

        // 싱글톤 인스턴스를 반환하는 정적 메서드
        static RenderManager& Instance()
        {
            static RenderManager renderManager;
            return renderManager;
        }

        // 렌더 객체들을 저장하는 해시맵 (렌더 객체의 고유 식별자, Render 객체의 스마트 포인터)
        std::unordered_map<std::string, std::shared_ptr<Render>> renders;
    };
}