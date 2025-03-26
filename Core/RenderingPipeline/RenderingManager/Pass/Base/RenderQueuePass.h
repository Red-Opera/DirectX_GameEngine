#pragma once

#include "RenderJob.h"
#include "RenderingPass.h"

#include <vector>

namespace RenderGraphNameSpace
{
	class RenderQueuePass : public RenderingPass
	{
	public:
		using RenderingPass::RenderingPass;

		void Accept(RenderJob renderJob) noexcept;

		void Reset() NOEXCEPTRELEASE override;

		// RenderingPass을(를) 통해 상속됨
		void Execute() NOEXCEPTRELEASE override;

		// 모든 RenderJob에 View Frustum 설정
		void SetViewFrustum(const CameraViewFrustumCulling& viewFrustum);

	protected:
		bool isFrustumCulling = true;

	private:
		std::vector<RenderJob> renderJobs;
	};
}