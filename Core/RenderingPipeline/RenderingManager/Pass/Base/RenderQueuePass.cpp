#include "stdafx.h"
#include "RenderQueuePass.h"

namespace RenderGraphNameSpace
{
	void RenderQueuePass::Accept(RenderJob renderJob) noexcept
	{
		renderJobs.push_back(renderJob);
	}

	void RenderQueuePass::Reset() NOEXCEPTRELEASE
	{
		renderJobs.clear();
	}

	void RenderQueuePass::Execute() const NOEXCEPTRELEASE
	{
		RenderAll();

		for (const auto& job : renderJobs)
			job.Excute();
	}
}