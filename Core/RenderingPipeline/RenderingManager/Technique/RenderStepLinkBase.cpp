#include "stdafx.h"
#include "RenderStepLinkBase.h"
#include "RenderStep.h"

#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"

void RenderStepLinkBase::OnSetRenderStep()
{
	renderStep->Link(renderGraph);
}
