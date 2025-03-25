#include "stdafx.h"
#include "RenderJob.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/Draw/OcclusionCulling.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/RenderStep.h"
#include "Core/Window.h"

namespace RenderGraphNameSpace
{
	CameraViewFrustumCulling RenderJob::m_viewFrustum;
	bool RenderJob::m_useFrustumCulling;

	std::unique_ptr<OcclusionCulling> RenderJob::m_occlusionCulling;
    
    RenderJob::RenderJob(const RenderStep* renderStep, const Drawable* drawable)
        : drawable{ drawable }, renderStep{ renderStep }
    {
        EnableOcclusionCulling(true);
        EnableFrustumCulling(true);
    }

    void RenderJob::Excute() NOEXCEPTRELEASE
    {
        // Frustum Culling 체크 추가
        if (!IsInViewFrustum())
            return;

        drawable->SetRenderPipeline();
        renderStep->SetRenderPipeline();

        Window::GetDxGraphic().DrawIndexed(drawable->GetIndexCount());
    }

    void RenderJob::EnableOcclusionCulling(bool enable)
    {
        m_useOcclusionCulling = enable;

        // OcclusionCulling 객체 지연 생성
        if (enable && m_occlusionCulling == nullptr)
        {
            m_occlusionCulling = std::make_unique<OcclusionCulling>();
        }
    }

    bool RenderJob::IsOcclusionCullingEnabled() const
    {
        return m_useOcclusionCulling && m_occlusionCulling != nullptr;
    }

    void RenderJob::BeginOcclusionQuery()
    {
        if (IsOcclusionCullingEnabled())
        {
            m_occlusionCulling->BeginQuery();
        }
    }

    void RenderJob::EndOcclusionQuery()
    {
        if (IsOcclusionCullingEnabled())
        {
            m_occlusionCulling->EndQuery();
        }
    }

    bool RenderJob::IsVisibleAfterOcclusionTest() const
    {
        // Occlusion Culling이 비활성화되어 있으면 항상 보이는 것으로 간주
        if (!IsOcclusionCullingEnabled())
        {
            return true;
        }

        // Occlusion Query 결과 확인
        return m_occlusionCulling->IsVisible();
    }

    bool RenderJob::UpdateOcclusionVisibility()
    {
        if (!IsOcclusionCullingEnabled())
            return true;

        return m_occlusionCulling->UpdateVisibility();
    }

    void RenderJob::EnableFrustumCulling(bool enable)
    {
        m_useFrustumCulling = enable;
    }

    bool RenderJob::IsFrustumCullingEnabled() const
    {
        return m_useFrustumCulling;
    }

    void RenderJob::SetViewFrustum(const CameraViewFrustumCulling& frustum)
    {
        m_viewFrustum = frustum;
    }

    CameraViewFrustumCulling& RenderGraphNameSpace::RenderJob::GetViewFrustum()
    {
		return m_viewFrustum;
    }

    bool RenderJob::IsInViewFrustum() const
    {
        if (!m_useFrustumCulling)
            return true;

        return drawable->IsInViewFrustum(m_viewFrustum);
    }
}
