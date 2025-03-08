#include "stdafx.h"
#include "RenderStep.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderQueuePass.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

RenderStep::RenderStep(std::string targetPassName) : targetPassName(std::move(targetPassName))
{

}

RenderStep::RenderStep(const RenderStep& renderStep) noexcept : targetPassName(renderStep.targetPassName)
{
	renders.reserve(renderStep.renders.size());

	for (auto& render : renderStep.renders)
	{
		if (auto* instance = dynamic_cast<const Graphic::RenderInstance*>(render.get()))
			renders.push_back(instance->Instance());

		else
			renders.push_back(render);
	}
}

void RenderStep::AddRender(std::shared_ptr<Graphic::Render> render) noexcept
{
	renders.push_back(std::move(render));
}

void RenderStep::Submit(const Drawable& drawable) const
{
	renderQueuePass->Accept(RenderGraphNameSpace::RenderJob{ this, &drawable });
}

void RenderStep::Accept(TechniqueBase& tech)
{
	tech.SetRenderStep(this);

	for (auto& render : renders)
		render->Accept(tech);
}

void RenderStep::Link(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	assert(renderQueuePass == nullptr);

	renderQueuePass = &renderGraph.GetRenderQueue(targetPassName);
}

void RenderStep::SetRenderPipeline() const NOEXCEPTRELEASE
{
	for (const auto& renderStep : renders)
		renderStep->SetRenderPipeline();
}

void RenderStep::InitializeParentReference(const Drawable& parent) noexcept
{
	for (auto& render : renders)
		render->InitializeParentReference(parent);
}
