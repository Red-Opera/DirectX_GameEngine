#include "stdafx.h"
#include "RenderQueuePass.h"

#include "Core/Exception/GraphicsException.h"
#include <sstream>

namespace RenderGraphNameSpace
{
    // 렌더 작업을 큐에 추가
    void RenderQueuePass::Accept(RenderJob renderJob) noexcept
    {
        // 렌더 작업을 큐의 끝에 추가 (이동 시맨틱 사용)
        renderJobs.push_back(std::move(renderJob));
    }

    // 모든 렌더 작업에 뷰 절두체 설정
    void RenderQueuePass::SetViewFrustum(const CameraViewFrustumCulling& viewFrustum)
    {
        // 큐에 있는 모든 렌더 작업에 뷰 절두체 설정
        for (auto& job : renderJobs)
            job.SetViewFrustum(viewFrustum);
    }

    // 패스 리셋 (모든 렌더 작업 제거)
    void RenderQueuePass::Reset() NOEXCEPTRELEASE
    {
        // 렌더 작업 큐 완전 초기화
        renderJobs.clear();
        
        // 메모리 최적화를 위해 벡터 용량도 축소
        renderJobs.shrink_to_fit();
    }

    // 큐에 있는 모든 렌더 작업 실행
    void RenderQueuePass::Execute() NOEXCEPTRELEASE
    {
        // 1. 먼저 부모 클래스의 렌더링 파이프라인 설정 (버퍼, 셰이더 등)
        RenderAll();

        // 2. 큐에 있는 모든 렌더 작업을 순서대로 실행 (각 작업에 절두체 컬링 설정을 전달하여 실행)
        for (RenderJob& job : renderJobs)
            job.Excute(isFrustumCulling);
    }
}