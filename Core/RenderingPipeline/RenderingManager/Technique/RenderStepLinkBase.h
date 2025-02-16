#pragma once

#include "TechniqueBase.h"

namespace RenderGraphNameSpace { class RenderGraph; }

class RenderStepLinkBase : public TechniqueBase
{
protected:
	void OnSetRenderStep() override;

private:
	RenderGraphNameSpace::RenderGraph& renderGraph;
};