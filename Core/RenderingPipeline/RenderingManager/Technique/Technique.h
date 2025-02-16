#pragma once

#include "RenderStep.h"

#include <vector>

namespace RenderGraphNameSpace { class RenderGraph; }

class TechniqueBase;
class Drawable;

class Technique
{
public:
	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept;

	void Submit(const Drawable& drawable) const noexcept;

	void push_back(RenderStep renderStep) noexcept;
	void SetActive(bool active);
	bool GetAcive() const noexcept;
	const std::string GetName() const noexcept;

	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Link(RenderGraphNameSpace::RenderGraph&);

	void Accept(TechniqueBase& tech);

private:
	std::vector<RenderStep> renderSteps;
	std::string name;

	bool isActive = true;
};