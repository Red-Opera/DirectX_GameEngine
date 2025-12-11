#include "stdafx.h"
#include "RenderJob.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/Draw/OcclusionCulling.h"
#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/RenderStep.h"
#include "Core/Window.h"

#include <sstream>

namespace RenderGraphNameSpace
{
    // 정적 멤버 초기화
    CameraViewFrustumCulling RenderJob::viewFrustumCulling;
    bool RenderJob::useViewFrustum = true;
    std::unique_ptr<OcclusionCulling> RenderJob::m_occlusionCulling;
    
    // 렌더 스텝과 그릴 객체로 렌더 작업 생성
    RenderJob::RenderJob(const RenderStep* renderStep, const Drawable* drawable)
        : drawable{ drawable }, renderStep{ renderStep }
    {
        // 입력 유효성 검증
        if (drawable == nullptr)
        {
            std::ostringstream oss;
            oss << "렌더 작업 생성 오류: Drawable 객체가 null입니다.\n"
                << "유효한 Drawable 객체를 제공해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        if (renderStep == nullptr)
        {
            std::ostringstream oss;
            oss << "렌더 작업 생성 오류: RenderStep 객체가 null입니다.\n"
                << "유효한 RenderStep 객체를 제공해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 기본적으로 오클루전 컬링 활성화
        EnableOcclusionCulling(true);
    }

    // 렌더링 작업 실행 (절두체 컬링 옵션 포함)
    void RenderJob::Excute(bool isPassFrustumCulling) NOEXCEPTRELEASE
    {
        // 절두체 컬링 체크 (패스별 설정, 전역 설정, 실제 절두체 내 존재 여부)
        if (isPassFrustumCulling && useViewFrustum && !IsInViewFrustum())
        {
            // 절두체 밖에 있으면 렌더링 건너뛰기 (성능 최적화)
            return;
        }

        // 렌더링 파이프라인 설정
        drawable->SetRenderPipeline();  // 객체의 버텍스/인덱스 버퍼 설정
        renderStep->SetRenderPipeline(); // 렌더 스텝의 셰이더/상태 설정

        // 실제 드로우 콜 실행
        Window::GetDxGraphic().DrawIndexed(drawable->GetIndexCount());
    }

    // 오클루전 컬링 활성화/비활성화
    void RenderJob::EnableOcclusionCulling(bool enable)
    {
        m_useOcclusionCulling = enable;

        // 오클루전 컬링 객체 지연 생성 (필요할 때만 생성)
        if (enable && m_occlusionCulling == nullptr)
        {
            try
            {
                m_occlusionCulling = std::make_unique<OcclusionCulling>();
            }

            catch (const std::exception& e)
            {
                std::ostringstream oss;
                oss << "오클루전 컬링 초기화 오류: OcclusionCulling 객체 생성에 실패했습니다.\n"
                    << "오류 내용: " << e.what() << "\n"
                    << "DirectX 하드웨어 지원을 확인해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }
    }

    // 오클루전 컬링 상태 확인
    bool RenderJob::IsOcclusionCullingEnabled() const
    {
        return m_useOcclusionCulling && m_occlusionCulling != nullptr;
    }

    // 오클루전 쿼리 시작 (가시성 테스트 시작)
    void RenderJob::BeginOcclusionQuery()
    {
        if (IsOcclusionCullingEnabled())
        {
            try
            {
                m_occlusionCulling->BeginQuery();
            }

            catch (const std::exception& e)
            {
                std::ostringstream oss;
                oss << "오클루전 쿼리 시작 오류: 가시성 테스트 쿼리를 시작할 수 없습니다.\n"
                    << "오류 내용: " << e.what() << "\n"
                    << "DirectX 쿼리 지원을 확인해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }
    }

    // 오클루전 쿼리 종료 (가시성 테스트 종료)
    void RenderJob::EndOcclusionQuery()
    {
        if (IsOcclusionCullingEnabled())
        {
            try
            {
                m_occlusionCulling->EndQuery();
            }
            catch (const std::exception& e)
            {
                std::ostringstream oss;
                oss << "오클루전 쿼리 종료 오류: 가시성 테스트 쿼리를 종료할 수 없습니다.\n"
                    << "오류 내용: " << e.what() << "\n"
                    << "BeginQuery()가 먼저 호출되었는지 확인해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }
        }
    }

    // 오클루전 테스트 후 객체가 보이는지 확인
    bool RenderJob::IsVisibleAfterOcclusionTest() const
    {
        // 오클루전 컬링이 비활성화되어 있으면 항상 보이는 것으로 간주
        if (!IsOcclusionCullingEnabled())
            return true;

        // 오클루전 쿼리 결과 확인
        return m_occlusionCulling->IsVisible();
    }

    // 오클루전 가시성 상태 업데이트
    bool RenderJob::UpdateOcclusionVisibility()
    {
        if (!IsOcclusionCullingEnabled())
            return true;

        try
        {
            return m_occlusionCulling->UpdateVisibility();
        }

        catch (const std::exception& e)
        {
            std::ostringstream oss;
            oss << "오클루전 가시성 업데이트 오류: 가시성 상태를 업데이트할 수 없습니다.\n"
                << "오류 내용: " << e.what() << "\n"
                << "쿼리가 완료되지 않았을 수 있습니다.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    // 절두체 컬링 활성화/비활성화 (전역 설정)
    void RenderJob::EnableFrustumCulling(bool enable)
    {
        useViewFrustum = enable;
    }

    // 절두체 컬링 상태 확인
    bool RenderJob::IsFrustumCullingEnabled() const
    {
        return useViewFrustum;
    }

    // 뷰 절두체 설정 (전역 설정)
    void RenderJob::SetViewFrustum(const CameraViewFrustumCulling& frustum)
    {
        viewFrustumCulling = frustum;
    }

    // 현재 뷰 절두체 반환
    CameraViewFrustumCulling& RenderGraphNameSpace::RenderJob::GetViewFrustum()
    {
        return viewFrustumCulling;
    }

    // 객체가 뷰 절두체 내에 있는지 확인
    bool RenderJob::IsInViewFrustum() const
    {
        // 절두체 컬링이 비활성화되어 있으면 항상 보이는 것으로 간주
        if (!useViewFrustum)
            return true;

        // drawable이 null인지 추가 안전성 검사 (생성자에서 검증했지만 방어적 프로그래밍)
        if (drawable == nullptr)
            return false;

        // Drawable 객체에게 절두체 내 존재 여부 확인 요청
        return drawable->IsInViewFrustum(viewFrustumCulling);
    }
}
