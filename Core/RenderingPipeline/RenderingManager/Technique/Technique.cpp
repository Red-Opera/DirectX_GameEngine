#include "stdafx.h"
#include "Technique.h"
#include "TechniqueBase.h"

#include "Core/Draw/Base/Drawable.h"

Technique::Technique(std::string name, size_t channel, bool startActive) noexcept 
	: name(name), channel(channel), isActive(startActive)
{

}

void Technique::Submit(const Drawable& drawable, size_t channelFilter) const noexcept
{
	if (isActive && ((channel & channelFilter) != 0))
	{
		for (const auto& renderStep : renderSteps)
			renderStep.Submit(drawable);
	}
}

Technique::Technique(size_t channel) : channel{ channel }
{

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