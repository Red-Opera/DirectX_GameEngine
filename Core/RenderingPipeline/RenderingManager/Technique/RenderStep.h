#pragma once

#include <vector>
#include <memory>

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/Render.h"

namespace RenderGraphNameSpace
{
	class RenderQueuePass;
	class RenderGraph;
}

class TechniqueBase;
class Drawable;

class RenderStep
{
public:
	RenderStep(std::string targetPassName);
	RenderStep(RenderStep&&) = default;
	RenderStep(const RenderStep& renderStep) noexcept;

	RenderStep& operator=(const RenderStep&) = delete;
	RenderStep& operator=(RenderStep&&) = delete;

	void AddRender(std::shared_ptr<Graphic::Render> render) noexcept;

	void Submit(const Drawable& drawable) const;
	void Accept(TechniqueBase& tech);
	void Link(RenderGraphNameSpace::RenderGraph& renderGraph);

	void SetRenderPipeline(DxGraphic& graphic) const NOEXCEPTRELEASE;

	void InitializeParentReference(const class Drawable& parent) noexcept;

private:
	std::vector<std::shared_ptr<Graphic::Render>> renders;
	RenderGraphNameSpace::RenderQueuePass* renderQueuePass = nullptr;

	std::string targetPassName;
};