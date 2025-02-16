#include "stdafx.h"
#include "RenderJob.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/RenderStep.h"

namespace RenderGraphNameSpace
{
	RenderJob::RenderJob(const RenderStep* renderStep, const Drawable* drawable)
		: drawable{ drawable }, renderStep{ renderStep }
	{

	}

	void RenderJob::Excute(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		drawable->SetRenderPipeline(graphic);
		renderStep->SetRenderPipeline(graphic);

		graphic.DrawIndexed(drawable->GetIndexCount());
	}
}