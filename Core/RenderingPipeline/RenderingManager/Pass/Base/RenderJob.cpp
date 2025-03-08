#include "stdafx.h"
#include "RenderJob.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/RenderStep.h"
#include "Core/Window.h"

namespace RenderGraphNameSpace
{
	RenderJob::RenderJob(const RenderStep* renderStep, const Drawable* drawable)
		: drawable{ drawable }, renderStep{ renderStep }
	{

	}

	void RenderJob::Excute() const NOEXCEPTRELEASE
	{
		drawable->SetRenderPipeline();
		renderStep->SetRenderPipeline();

		Window::GetDxGraphic().DrawIndexed(drawable->GetIndexCount());
	}
}