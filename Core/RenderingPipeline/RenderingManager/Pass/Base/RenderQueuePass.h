#pragma once

#include "RenderJob.h"
#include "RenderingPass.h"

#include <vector>

namespace RenderGraphNameSpace
{
    // 렌더링 작업들을 큐에 수집하고 일괄 실행하는 렌더 패스 클래스
    class RenderQueuePass : public RenderingPass
    {
    public:
        // 부모 클래스의 생성자들을 그대로 사용
        using RenderingPass::RenderingPass;

        // 렌더 작업을 큐에 추가
        void Accept(RenderJob renderJob) noexcept;

        // 패스 리셋 (모든 렌더 작업 제거)
        void Reset() NOEXCEPTRELEASE override;

        // 큐에 있는 모든 렌더 작업 실행
        void Execute() NOEXCEPTRELEASE override;

        // 모든 렌더 작업에 뷰 절두체 설정
        void SetViewFrustum(const CameraViewFrustumCulling& viewFrustum);

    protected:
        bool isFrustumCulling = true;       // 절두체 컬링 사용 여부

    private:
        std::vector<RenderJob> renderJobs;  // 실행할 렌더 작업 큐
    };
}