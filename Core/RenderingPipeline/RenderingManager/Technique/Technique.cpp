#include "stdafx.h"
#include "Technique.h"
#include "TechniqueBase.h"

#include "Core/Draw/Base/Drawable.h"

Technique::Technique(std::string name, bool startActive) noexcept : name(name), isActive(startActive) 
{

}

void Technique::Submit(const Drawable& drawable) const noexcept
{
	if (isActive)
	{
		for (const auto& renderStep : renderSteps)
			renderStep.Submit(drawable);
	}
}

void Technique::push_back(RenderStep renderStep) noexcept
{
	renderSteps.push_back(std::move(renderStep));
}

void Technique::SetActive(bool active)
{
	isActive = active;
}

bool Technique::GetAcive() const noexcept
{
	return isActive;
}

const std::string Technique::GetName() const noexcept
{
	return name;
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& renderStep : renderSteps)
		renderStep.InitializeParentReference(parent);
}

void Technique::Link(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& step : renderSteps)
		step.Link(renderGraph);
}

void Technique::Accept(TechniqueBase& tech)
{
	tech.SetTechnique(this);

	for (auto& step : renderSteps)
		step.Accept(tech);
}