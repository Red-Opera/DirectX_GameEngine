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

		// RenderingPass��(��) ���� ��ӵ�
		void Execute() const NOEXCEPTRELEASE override;

	private:
		std::vector<RenderJob> renderJobs;
	};
}