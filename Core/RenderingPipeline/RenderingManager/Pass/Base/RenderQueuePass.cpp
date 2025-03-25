#include "stdafx.h"
#include "RenderQueuePass.h"

namespace RenderGraphNameSpace
{
	void RenderQueuePass::Accept(RenderJob renderJob) noexcept
	{
		renderJobs.push_back(std::move(renderJob));
	}

	void RenderQueuePass::SetViewFrustum(const CameraViewFrustumCulling& viewFrustum)
	{
		for (auto& job : renderJobs)
			job.SetViewFrustum(viewFrustum);
	}

	void RenderQueuePass::Reset() NOEXCEPTRELEASE
	{
		renderJobs.clear();
	}

	void RenderQueuePass::Execute() NOEXCEPTRELEASE
	{
		RenderAll();

		// 렌더링 작업 수행 (쿼리 결과를 기다리지 않고 비동기적으로 처리)
		for (RenderJob& job : renderJobs)
			job.Excute();
	}
}