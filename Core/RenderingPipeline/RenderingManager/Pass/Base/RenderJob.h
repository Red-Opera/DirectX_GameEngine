#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/Draw/OcclusionCulling.h"
#include "Core/Camera/CameraViewFrustumCulling.h"

#include <memory>

class Drawable;
class DxGraphic;
class RenderStep;

namespace RenderGraphNameSpace
{
    // 실제 렌더링 작업을 수행하는 작업 단위 클래스
    class RenderJob
    {
    public:
        // 렌더 스텝과 그릴 객체로 렌더 작업 생성
        RenderJob(const RenderStep* renderStep, const Drawable* drawable);

        // 렌더링 작업 실행 (절두체 컬링 옵션 포함)
        void Excute(bool isPassFrustumCulling = true) NOEXCEPTRELEASE;

        void EnableOcclusionCulling(bool enable);   // 오클루전 컬링 활성화/비활성화
        bool IsOcclusionCullingEnabled() const;     // 오클루전 컬링 상태 확인

        void BeginOcclusionQuery();                 // 오클루전 쿼리 시작 (가시성 테스트 시작)
        void EndOcclusionQuery();                   // 오클루전 쿼리 종료 (가시성 테스트 종료)

        bool IsVisibleAfterOcclusionTest() const;   // 오클루전 테스트 후 객체가 보이는지 확인
        bool UpdateOcclusionVisibility();           // 오클루전 가시성 상태 업데이트

        void EnableFrustumCulling(bool enable);     // 절두체 컬링 활성화/비활성화 (전역 설정)
        bool IsFrustumCullingEnabled() const;       // 절두체 컬링 상태 확인

        static void SetViewFrustum(const CameraViewFrustumCulling& frustum);    // 뷰 절두체 설정 (전역 설정)
        static CameraViewFrustumCulling& GetViewFrustum();                      // 현재 뷰 절두체 반환

        // 객체가 뷰 절두체 내에 있는지 확인
        bool IsInViewFrustum() const;

    private:
        const class Drawable* drawable;     // 렌더링할 객체
        const class RenderStep* renderStep; // 렌더링 스텝

        // 오클루전 컬링 관련 멤버
        static std::unique_ptr<class OcclusionCulling> m_occlusionCulling;  // 오클루전 컬링 객체 (지연 생성)
        bool m_useOcclusionCulling = false;                                 // 오클루전 컬링 사용 여부

        // 절두체 컬링 관련 정적 멤버
        static CameraViewFrustumCulling viewFrustumCulling;     // 전역 뷰 절두체
        static bool useViewFrustum;                             // 절두체 컬링 사용 여부
    };
}